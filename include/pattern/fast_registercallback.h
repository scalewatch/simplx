/**
 * @file fast_callback.h
 * @brief Simplx pattern allowing a more efficient callback mechanism than original one
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#include "simplx.h"

using namespace tredzone;

class FastRegisterCallback
{
  public:
	virtual void onCallback() {};
};

// this (core)-singleton manage the callback mechanism. (One FastCallbackSingleton per core)
class FastCallbackSingleton : public Actor, public Actor::Callback
{
  public:
	void fastRegisterCallback(FastRegisterCallback &source)
	{
		callbacks.push_front(&source);
		if (!isFastRegistered)
		{
			isFastRegistered = true;
			registerCallback(*this);
		}
	}

	void onCallback()
	{
		isFastRegistered = false;
		callbacks.swap(inception_callbacks);
		for (auto source : inception_callbacks)
		{
			source->onCallback();
		}

		inception_callbacks.clear();
	}

  private:

	// two lists are used to avoid infinite recursion between callback-registration and callback-call
	std::list<FastRegisterCallback *> callbacks;
	std::list<FastRegisterCallback *> inception_callbacks;
	bool isFastRegistered = false;
};

// wrapper part of the fast-callback mechanism
class FastCallbackProxy : virtual public FastRegisterCallback
{
  public:
	FastCallbackProxy(Actor &source)
	{
		// singleton access is wrapped here
		ref = source.newReferencedSingletonActor<FastCallbackSingleton>();
	}

	void fastRegisterCallback(FastRegisterCallback &source)
	{
		// callback registration is wrapped here
		ref->fastRegisterCallback(source);
	}

  private:
	Actor::ActorReference<FastCallbackSingleton> ref;
};