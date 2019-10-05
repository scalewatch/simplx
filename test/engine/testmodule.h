/**
 * @file testmodule.h
 * @brief testing class
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#pragma once

#include "testutil.h"

#include "simplx_core/engine.h"

class TestModule: public simplx::Actor, private simplx::Actor::Callback
{
public:
	class TestMilestone: public simplx::MultiForwardChainLink<TestMilestone>
    {
	public:
		TestMilestone() :
				startedFlag(false) {
		}
		virtual ~TestMilestone() {
		}

	private:
		friend class TestModule;
		bool startedFlag;

		virtual void start() = 0;
		virtual bool processNext() = 0;
	};

	inline TestModule()
    {
		registerCallback(*this);
	}

protected:

	TestMilestone::ForwardChain<> testMilestoneChain;

private:
	friend class simplx::Actor;
	virtual void onDestroyRequest() noexcept {
		if (testMilestoneChain.empty())
        {
			acceptDestroy();
		}
	}
	void onCallback() noexcept {
		_SIMPLX_TEST_EXIT_EXCEPTION_CATCH_BEGIN_

			registerCallback(*this);
			if (!testMilestoneChain.front()->startedFlag) {
				testMilestoneChain.front()->startedFlag = true;
				testMilestoneChain.front()->start();
			} else if (testMilestoneChain.front()->processNext()) {
				delete testMilestoneChain.pop_front();
				if (testMilestoneChain.empty())
                {
					requestDestroy();
				}
			}

		_SIMPLX_TEST_EXIT_EXCEPTION_CATCH_END_
	}
};

