/**
 * @file testasyncactor.cpp
 * @brief test actor functionality
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#include <set>

#include "gtest/gtest.h"

#include "testutil.h"

using namespace std;
using namespace simplx;

// anonymous namespace to prevent gcc linker to confuse/mix identically-name classes
namespace
{

class TestUndeliveredActor : public simplx::Actor, public simplx::Actor::Callback
{
public:
    TestUndeliveredActor() : undeliveredEventCount(0)
    {
        cout << "TestUndeliveredActor::TestUndeliveredActor(), otherActorId=" << otherActorId << endl;
        registerUndeliveredEventHandler<Event>(*this);
        onCallback();
    }
    TestUndeliveredActor(int) : otherActorId(newUnreferencedActor<simplx::Actor>()), undeliveredEventCount(0)
    {
        cout << "TestUndeliveredActor::TestUndeliveredActor(), otherActorId=" << otherActorId << endl;
        registerUndeliveredEventHandler<Event>(*this);
        onCallback();
    }
    virtual ~TestUndeliveredActor() noexcept
    {
        cout << "TestUndeliveredActor::~TestUndeliveredActor()" << endl;
    }
    void onUndeliveredEvent(const Event &event)
    {
        cout << "TestUndeliveredActor::onUndeliveredEvent()" << endl;
        ASSERT_EQ(event.getSourceActorId(), *this);
        ++undeliveredEventCount;
    }
    void onCallback() noexcept
    {
        registerCallback(*this);
        Event::Pipe(*this, otherActorId).push<Event>();
    }

  protected:
    virtual void onDestroyRequest() noexcept
    {
        if (undeliveredEventCount == 0)
        {
            requestDestroy();
        }
        else
        {
            simplx::Actor::onDestroyRequest();
        }
    }

  private:
    const ActorId otherActorId;
    size_t undeliveredEventCount;
};
/**
 * @file testasyncactor.cpp
 * @brief test actor functionality
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */


void testUndelivered()
{
    {
        simplx::Engine::StartSequence startSequence;
        startSequence.addActor<TestUndeliveredActor>(0);
        simplx::TestEngine engine(startSequence);
    }
    {
        simplx::Engine::StartSequence startSequence;
        startSequence.addActor<TestUndeliveredActor>(0, 0);
        simplx::TestEngine engine(startSequence);
    }
}

struct SimpleServiceActor : public simplx::Actor
{
    struct Tag : simplx::Service
    {
    };

    bool &serviceDestroyedFlag;

    SimpleServiceActor(bool *serviceDestroyedFlag) : serviceDestroyedFlag(*serviceDestroyedFlag)
    {
        EXPECT_FALSE(this->serviceDestroyedFlag);
        cout << "SimpleServiceActor::SimpleServiceActor()" << endl;
    }
    ~SimpleServiceActor() noexcept
    {
        EXPECT_TRUE(serviceDestroyedFlag);
        cout << "SimpleServiceActor::~SimpleServiceActor()" << endl;
    }
    void onDestroyRequest() noexcept
    {
        ASSERT_FALSE(serviceDestroyedFlag);
        serviceDestroyedFlag = true;
        cout << "SimpleServiceActor::onDestroyRequest()" << endl;
        simplx::Actor::onDestroyRequest();
    }
};

struct MyActor : public simplx::Actor
{
    struct EternalActor : public simplx::Actor
    {
        bool &serviceDestroyedFlag;
        unsigned destroyRetryCount;

        EternalActor(bool *serviceDestroyedFlag) : serviceDestroyedFlag(*serviceDestroyedFlag), destroyRetryCount(10)
        {
            cout << "EternalActor::EternalActor()" << endl;
        }
        ~EternalActor() noexcept
        {
            EXPECT_FALSE(serviceDestroyedFlag);
            cout << "EternalActor::~EternalActor()" << endl;
        }
        void onDestroyRequest() noexcept
        {
            ASSERT_GT(destroyRetryCount, 0u);
            if (--destroyRetryCount == 0u)
            {
                // accept
                cout << "EternalActor::onDestroyRequest() accepted" << endl;
                acceptDestroy();
            }
            else
            {   // postponed
                cout << "EternalActor::onDestroyRequest() refused" << endl;
                requestDestroy();
            }
        }
    };

    MyActor(bool *serviceDestroyedFlag)
    {
        cout << "MyActor::MyActor()" << endl;
        eternal = newReferencedSingletonActor<EternalActor>(serviceDestroyedFlag);
    }
    ~MyActor() throw() { cout << "MyActor::~MyActor()" << endl; }
    
    void onDestroyRequest() noexcept override
    {
        cout << "MyActor::onDestroyRequest" << endl;
        acceptDestroy();
    }

    ActorReference<EternalActor> eternal;
};

void testActorReference()
{
    bool serviceDestroyedFlag = false;
    simplx::Engine::StartSequence startSequence;
    startSequence.addServiceActor<SimpleServiceActor::Tag, SimpleServiceActor>(0, &serviceDestroyedFlag);
    startSequence.addActor<MyActor>(0, &serviceDestroyedFlag);
    {
        simplx::Engine engine(startSequence);
    }
}

struct ActorMultipleReferenceTest : simplx::Actor
{
    struct Tag1 : simplx::Service
    {
    };
    struct Tag2 : simplx::Service
    {
    };
    struct Tag3 : simplx::Service
    {
    };
    struct DestroyFlags
    {
        static unsigned lastDestroyedServiceId;
        static void reportRegularActorDestroy()
        {
            ASSERT_EQ(0u, lastDestroyedServiceId);
            cout << "ActorMultipleReferenceTest::DestroyFlags::reportRegularActorDestroy()" << endl;
        }
        static void reportServiceDestroyed(unsigned serviceId)
        {
            ASSERT_EQ(serviceId, lastDestroyedServiceId + 1);
            lastDestroyedServiceId = serviceId;
            cout << "ActorMultipleReferenceTest::DestroyFlags::reportServiceDestroyed(), serviceId=" << serviceId
                      << endl;
        }
        static void reportServiceDependentDestroyed(unsigned serviceId)
        {
            ASSERT_EQ(lastDestroyedServiceId, serviceId);
            cout << "ActorMultipleReferenceTest::DestroyFlags::reportServiceDependentDestroyed(), serviceId="
                      << serviceId << endl;
        }
    };

    struct InnerActor : simplx::Actor
    {
        InnerActor(const ActorId &parentActorId)
            : actorReference(referenceLocalActor<simplx::Actor>(parentActorId)), destroyRetryCount(3)
        {
            cout << "ActorMultipleReferenceTest::ActorMultipleReferenceTest::InnerActor()" << endl;
        }
        virtual ~InnerActor() throw()
        {
            DestroyFlags::reportRegularActorDestroy();
            cout << "ActorMultipleReferenceTest::ActorMultipleReferenceTest::~InnerActor()" << endl;
        }
        virtual void onDestroyRequest() throw()
        {
            ASSERT_GT(destroyRetryCount, 0u);
            if (--destroyRetryCount == 0)
            {
                simplx::Actor::onDestroyRequest();
            }
            else
            {
                cout << "ActorMultipleReferenceTest::InnerActor::onDestroyRequest() refused" << endl;
                requestDestroy();
            }
        }
        ActorReference<simplx::Actor> actorReference;
        unsigned destroyRetryCount;
    };

    const unsigned serviceId;
    unsigned destroyRetryCount;

    ActorMultipleReferenceTest(unsigned serviceId = 0) : serviceId(serviceId), destroyRetryCount(3)
    {
        newUnreferencedActor<InnerActor>(getActorId());
        newUnreferencedActor<InnerActor>(getActorId());
        newUnreferencedActor<InnerActor>(getActorId());
        cout << "ActorMultipleReferenceTest::ActorMultipleReferenceTest(" << this << ")"
                  << (serviceId == 0 ? "" : " [service]") << endl;
    }
    virtual ~ActorMultipleReferenceTest() throw()
    {
        if (serviceId != 0)
        {
            DestroyFlags::reportServiceDestroyed(serviceId);
        }
        else
        {
            DestroyFlags::reportRegularActorDestroy();
        }
        cout << "ActorMultipleReferenceTest::~ActorMultipleReferenceTest(" << this << ")"
                  << (serviceId == 0 ? "" : " [service]") << endl;
    }
    virtual void onDestroyRequest() throw()
    {
        ASSERT_GT(destroyRetryCount, 0u);
        if (--destroyRetryCount == 0)
        {
            simplx::Actor::onDestroyRequest();
        }
        else
        {
            cout << "ActorMultipleReferenceTest::onDestroyRequest(" << this << ") refused"
                      << (serviceId == 0 ? "" : " [service]") << endl;
            requestDestroy();
        }
    }
};

unsigned ActorMultipleReferenceTest::DestroyFlags::lastDestroyedServiceId = 0;

struct ActorMultipleReferenceTest2 : ActorMultipleReferenceTest
{
    struct InnerSingletonActor : simplx::Actor
    {
        InnerSingletonActor(const pair<ActorId, unsigned> &init)
            : actorReference(referenceLocalActor<ActorMultipleReferenceTest>(init.first)), serviceId(init.second),
              destroyRetryCount(3)
        {
            cout << "ActorMultipleReferenceTest::ActorMultipleReferenceTest2::InnerSingletonActor()" << endl;
        }
        virtual ~InnerSingletonActor() throw()
        {
            DestroyFlags::reportServiceDependentDestroyed(serviceId);
            cout << "ActorMultipleReferenceTest::ActorMultipleReferenceTest2::~InnerSingletonActor()" << endl;
        }
        virtual void onDestroyRequest() throw()
        {
            ASSERT_GT(destroyRetryCount, 0u);
            if (--destroyRetryCount == 0)
            {
                simplx::Actor::onDestroyRequest();
            }
            else
            {
                cout << "ActorMultipleReferenceTest2::InnerSingletonActor::onDestroyRequest() refused" << endl;
                requestDestroy();
            }
        }

        ActorReference<ActorMultipleReferenceTest> actorReference;
        const unsigned serviceId;
        unsigned destroyRetryCount;
    };

    ActorMultipleReferenceTest2(unsigned serviceId)
        : ActorMultipleReferenceTest(serviceId),
          actorReference(referenceLocalActor<simplx::Actor>(getEngine().getServiceIndex().getServiceActorId<Tag1>())),
          singletonActorReference(newReferencedSingletonActor<InnerSingletonActor>(
              make_pair(getEngine().getServiceIndex().getServiceActorId<Tag1>(), serviceId)))
    {
    }

    ActorReference<simplx::Actor> actorReference;
    ActorReference<InnerActor> singletonActorReference;
};

void testActorMultipleReference()
{
    simplx::Engine::StartSequence startSequence;
    startSequence.addServiceActor<ActorMultipleReferenceTest::Tag1, ActorMultipleReferenceTest>(0, 3u);
    startSequence.addServiceActor<ActorMultipleReferenceTest::Tag2, ActorMultipleReferenceTest2>(0, 2u);
    startSequence.addServiceActor<ActorMultipleReferenceTest::Tag3, ActorMultipleReferenceTest2>(0, 1u);
    startSequence.addActor<ActorMultipleReferenceTest>(0);
    {
        simplx::Engine engine(startSequence);
    }
}

class ActorIds
{
  public:
    struct Actor : simplx::Actor
    {
        Actor(ActorIds *actorIds)
        {
            actorIds->setActorId(*this);
            cout << "ActorIds::Actor::Actor() [service], this=" << this << endl;
        }
        virtual ~Actor() throw() { cout << "ActorIds::Actor::~Actor() [service], this=" << this << endl; }
    };
    typedef set<simplx::Actor::ActorId> ActorIdSet;

    ActorIds() : engineStopFlag(false), getCoreActorIdsCalledFlag(false), mainThreadId(simplx::ThreadId::current()) {}
    ~ActorIds() { EXPECT_EQ(mainThreadId, simplx::ThreadId::current()); }
    void setActorId(const simplx::Actor::ActorId &actorId)
    {
        ASSERT_EQ(mainThreadId, simplx::ThreadId::current());
        ASSERT_FALSE(getCoreActorIdsCalledFlag);
        actorIdSet.insert(actorId);
        cout << "ActorIds::setActorId(), actorId=" << actorId << endl;
    }
    const ActorIdSet &getCoreActorIds() const throw()
    {
        EXPECT_NE(mainThreadId, simplx::ThreadId::current());
        getCoreActorIdsCalledFlag = true;
        return actorIdSet;
    }
    void stopEngine() const { engineStopFlag = true; }
    bool canStopEngine() const { return engineStopFlag; }

  private:
    volatile mutable bool engineStopFlag;
    volatile mutable bool getCoreActorIdsCalledFlag;
    simplx::ThreadId mainThreadId;
    ActorIdSet actorIdSet;
};

struct AbstractActorReferenceTreeFactory
{
    virtual ~AbstractActorReferenceTreeFactory() throw() {}
    virtual simplx::Actor::ActorReference<simplx::Actor> newActorReference(simplx::Actor &) const = 0;
    virtual AbstractActorReferenceTreeFactory *clone() const = 0;
};

class ActorReferenceTreeNodeActor : public simplx::Actor
{
  public:
    ActorReferenceTreeNodeActor(const AbstractActorReferenceTreeFactory *treeFactory)
        : actorReferenceList(getAllocator())
    {
        for (ActorReference<simplx::Actor> actorReference = treeFactory->newActorReference(*this);
             actorReference.get() != 0; actorReference = treeFactory->newActorReference(*this))
        {
            actorReferenceList.push_back(actorReference);
        }
    }
    virtual ~ActorReferenceTreeNodeActor() throw() {}

  private:
    list<ActorReference<simplx::Actor>, Allocator<ActorReference<simplx::Actor>>> actorReferenceList;
};

class TestDetectionOfEventLoopEndActor : public simplx::Actor, public simplx::Actor::Callback
{
  public:
    struct InnerActor : simplx::Actor
    {
        list<ActorReference<simplx::Actor>, Allocator<ActorReference<simplx::Actor>>> referenceActorList;
        InnerActor(const ActorIds *actorIds) : referenceActorList(getAllocator())
        {
            for (ActorIds::ActorIdSet::const_iterator i = actorIds->getCoreActorIds().begin();
                 i != actorIds->getCoreActorIds().end(); ++i)
            {
                referenceActorList.push_back(referenceLocalActor<simplx::Actor>(*i));
                cout << "TestDetectionOfEventLoopEndActor::InnerActor::InnerActor(), this=" << this
                          << ", referencing=" << *i << endl;
            }
            cout << "TestDetectionOfEventLoopEndActor::InnerActor::InnerActor(), this=" << this << endl;
        }
        virtual ~InnerActor() throw()
        {
            cout << "TestDetectionOfEventLoopEndActor::InnerActor::~InnerActor(), this=" << this << endl;
        }
    };
    typedef pair<const ActorIds *, const AbstractActorReferenceTreeFactory *> Init;
    TestDetectionOfEventLoopEndActor(const Init &init) : actorIds(*init.first), treeFactoryPtr(init.second->clone())
    {
        registerCallback(*this);
    }
    virtual ~TestDetectionOfEventLoopEndActor() throw() {}
    void onCallback() throw()
    {
        actor1 = treeFactoryPtr->newActorReference(*this);
        actor2 = newReferencedSingletonActor<InnerSingleton>(treeFactoryPtr.get());
        actorIds.stopEngine();
    }

  protected:
    virtual void onDestroyRequest() throw()
    {
        if (isRegistered())
        {
            requestDestroy();
        }
        else
        {
            simplx::Actor::onDestroyRequest();
        }
    }

  private:
    struct InnerSingleton : simplx::Actor
    {
        ActorReference<ActorReferenceTreeNodeActor> actor;
        InnerSingleton(const AbstractActorReferenceTreeFactory *treeFactory)
            : actor(treeFactory->newActorReference(*this))
        {
        }
    };

    const ActorIds &actorIds;
    unique_ptr<AbstractActorReferenceTreeFactory> treeFactoryPtr;
    ActorReference<ActorReferenceTreeNodeActor> actor1;
    ActorReference<ActorReferenceTreeNodeActor> actor2;
};

void testDetectionOfEventLoopEnd(ActorIds &actorIds, const AbstractActorReferenceTreeFactory &treeFactory)
{
    struct CustumCoreFactory : simplx::EngineCustomCoreActorFactory
    {
        ActorIds &actorIds;
        CustumCoreFactory(ActorIds &actorIds) : actorIds(actorIds) {}
        virtual simplx::Actor::ActorReference<simplx::Actor> newCustomCoreActor(simplx::Engine &, simplx::Actor::CoreId, bool,
                                                                          simplx::Actor &parent)
        {
            actorIds.setActorId(parent);
            return parent.newReferencedActor<DefaultCoreActor>();
        }
    };

    CustumCoreFactory custumCoreFactory(actorIds);
    simplx::Engine::StartSequence startSequence;
    startSequence.setEngineCustomCoreActorFactory(custumCoreFactory);
    startSequence.addServiceActor<ActorMultipleReferenceTest::Tag1, ActorIds::Actor>(0, &actorIds);
    startSequence.addActor<TestDetectionOfEventLoopEndActor>(
        0, TestDetectionOfEventLoopEndActor::Init(&actorIds, &treeFactory));
    {
        for (simplx::Engine engine(startSequence); actorIds.canStopEngine() == false; simplx::threadSleep())
        {
        }
    }
}

void testDetectionOfEventLoopEnd()
{
    struct ActorReferenceTreeFactory : AbstractActorReferenceTreeFactory
    {
        const ActorIds &actorIds;
        ActorReferenceTreeFactory(const ActorIds &actorIds) : actorIds(actorIds) {}
        virtual simplx::Actor::ActorReference<simplx::Actor> newActorReference(simplx::Actor &parent) const
        {
            simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor> actor_l0 =
                parent.newReferencedActor<TestDetectionOfEventLoopEndActor::InnerActor>(&actorIds);
            simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor> actor1_l1 =
                actor_l0->newReferencedActor<TestDetectionOfEventLoopEndActor::InnerActor>(&actorIds);
            simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor> actor2_l1 =
                actor_l0->newReferencedActor<TestDetectionOfEventLoopEndActor::InnerActor>(&actorIds);
            simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor> actor1_l2 =
                actor1_l1->newReferencedActor<TestDetectionOfEventLoopEndActor::InnerActor>(&actorIds);
            simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor> actor2_l2 =
                actor1_l1->newReferencedActor<TestDetectionOfEventLoopEndActor::InnerActor>(&actorIds);
            newLeafActorReference(actor1_l1, actor1_l2, actor2_l2);
            newLeafActorReference(actor_l0, actor1_l1, actor2_l1);
            return actor_l0;
        }
        void newLeafActorReference(simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor> &parent,
                                   simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor> leaf1 =
                                       simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor>(),
                                   simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor> leaf2 =
                                       simplx::Actor::ActorReference<TestDetectionOfEventLoopEndActor::InnerActor>()) const
        {
            if (leaf1.get() != 0)
            {
                parent->referenceActorList.push_back(leaf1);
                referenceActorIds(*leaf1);
            }
            if (leaf2.get() != 0)
            {
                parent->referenceActorList.push_back(leaf2);
                referenceActorIds(*leaf2);
            }
        }
        virtual AbstractActorReferenceTreeFactory *clone() const { return new ActorReferenceTreeFactory(actorIds); }
        void referenceActorIds(TestDetectionOfEventLoopEndActor::InnerActor &actor) const
        {
            for (ActorIds::ActorIdSet::const_iterator i = actorIds.getCoreActorIds().begin();
                 i != actorIds.getCoreActorIds().end(); ++i)
            {
                actor.referenceActorList.push_back(actor.referenceLocalActor<simplx::Actor>(*i));
            }
        }
    };
    ActorIds actorIds;
    ActorReferenceTreeFactory actorReferenceTreeFactory(actorIds);
    testDetectionOfEventLoopEnd(actorIds, actorReferenceTreeFactory);
}

struct TestLoopPerformanceNeutralActor : simplx::Actor
{
    struct PerformanceCounters
    {
        unsigned loopUsageCount;
        unsigned onEventCount;
        unsigned onCallbackCount;
        PerformanceCounters() : loopUsageCount(0), onEventCount(0), onCallbackCount(0) {}
        PerformanceCounters(unsigned loopUsageCount, unsigned onEventCount, unsigned onCallbackCount)
            : loopUsageCount(loopUsageCount), onEventCount(onEventCount), onCallbackCount(onCallbackCount)
        {
        }
        bool operator==(const PerformanceCounters &other) const
        {
            return loopUsageCount == other.loopUsageCount && onEventCount == other.onEventCount &&
                   onCallbackCount == other.onCallbackCount;
        }
    };
    class ActorIdSetThreadSafe
    {
      public:
        static ActorIdSetThreadSafe instance;
        void add(const ActorId &actorId)
        {
            simplx::Mutex::Lock lock(mutex);
            actorIdSet.insert(actorId);
        }
        void remove(const ActorId &actorId) throw()
        {
            simplx::Mutex::Lock lock(mutex);
            actorIdSet.erase(actorId);
        }
        ActorId getOtherThan(const ActorId &actorId)
        {
            simplx::Mutex::Lock lock(mutex);
            for (ActorIdSet::const_iterator i = actorIdSet.begin(), endi = actorIdSet.end(); i != endi; ++i)
            {
                if (*i != actorId)
                {
                    return *i;
                }
            }
            return ActorId();
        }

      private:
        simplx::Mutex mutex;
        typedef set<ActorId> ActorIdSet;
        ActorIdSet actorIdSet;
        ActorIdSetThreadSafe() {}
    };

    PerformanceCounters &performanceCounters;

    TestLoopPerformanceNeutralActor(PerformanceCounters &performanceCounters) : performanceCounters(performanceCounters)
    {
        ActorIdSetThreadSafe::instance.add(*this);
    }
    virtual ~TestLoopPerformanceNeutralActor() throw() { ActorIdSetThreadSafe::instance.remove(*this); }
    virtual void onDestroyRequest() throw()
    {
        cout << "TestLoopPerformanceNeutralActor::onDestroyRequest(), getCoreUsageLoopCount()="
                  << getCorePerformanceCounters().getLoopUsageCount() << endl;
        performanceCounters.loopUsageCount = (unsigned)getCorePerformanceCounters().getLoopUsageCount();
        performanceCounters.onEventCount = (unsigned)getCorePerformanceCounters().getOnEventCount();
        performanceCounters.onCallbackCount = (unsigned)getCorePerformanceCounters().getOnCallbackCount();
        ASSERT_GT(getCorePerformanceCounters().getLoopTotalCount(), performanceCounters.loopUsageCount);
        simplx::Actor::onDestroyRequest();
    }
};

TestLoopPerformanceNeutralActor::ActorIdSetThreadSafe TestLoopPerformanceNeutralActor::ActorIdSetThreadSafe::instance;

struct TestLoopPerformanceNeutralCallbackActor : TestLoopPerformanceNeutralActor, simplx::Actor::Callback
{
    TestLoopPerformanceNeutralCallbackActor(PerformanceCounters *performanceCounters)
        : TestLoopPerformanceNeutralActor(*performanceCounters)
    {
        registerPerformanceNeutralCallback(*this);
    }
    void onCallback() throw() { cout << "TestLoopPerformanceNeutralCallbackActor::onCallback()" << endl; }
};

struct TestLoopPerformanceCallbackActor : TestLoopPerformanceNeutralActor, simplx::Actor::Callback
{
    TestLoopPerformanceCallbackActor(PerformanceCounters *performanceCounters)
        : TestLoopPerformanceNeutralActor(*performanceCounters)
    {
        registerCallback(*this);
    }
    void onCallback() throw() { cout << "TestLoopPerformanceCallbackActor::onCallback()" << endl; }
};

struct TestLoopPerformanceDestroyActor : TestLoopPerformanceNeutralActor
{
    unsigned detroyRetryCount;
    TestLoopPerformanceDestroyActor(PerformanceCounters *performanceCounters)
        : TestLoopPerformanceNeutralActor(*performanceCounters), detroyRetryCount(0)
    {
        requestDestroy();
    }
    virtual void onDestroyRequest() throw()
    {
        cout << "TestLoopPerformanceDestroyActor::onDestroyRequest()" << endl;
        if (++detroyRetryCount == 2)
        {
            TestLoopPerformanceNeutralActor::onDestroyRequest();
        }
        else
        {
            requestDestroy();
        }
    }
};

struct TestLoopPerformanceLocalEventActor : TestLoopPerformanceNeutralActor
{
    TestLoopPerformanceLocalEventActor(PerformanceCounters *performanceCounters)
        : TestLoopPerformanceNeutralActor(*performanceCounters)
    {
        registerEventHandler<Event>(*this);
        Event::Pipe(*this, *this).push<Event>();
    }
    void onEvent(const Event &) throw() { cout << "TestLoopPerformanceLocalEventActor::onEvent()" << endl; }
};

struct TestLoopPerformanceLocalUndeliveredEventActor : TestLoopPerformanceNeutralActor
{
    TestLoopPerformanceLocalUndeliveredEventActor(PerformanceCounters *performanceCounters)
        : TestLoopPerformanceNeutralActor(*performanceCounters)
    {
        registerUndeliveredEventHandler<Event>(*this);
        Event::Pipe(*this).push<Event>();
    }
    void onUndeliveredEvent(const Event &) throw()
    {
        uint64_t outOfBoundWrittenSize =
            getCorePerformanceCounters().getTotalWrittenEventByteSizeTo((NodeId)getEngine().getCoreSet().size());
        uint64_t localWrittenSize =
            getCorePerformanceCounters().getTotalWrittenEventByteSizeTo(getActorId().getNodeId());
        uint64_t otherWrittenSize = getCorePerformanceCounters().getTotalWrittenEventByteSizeTo(
            TestLoopPerformanceNeutralActor::ActorIdSetThreadSafe::instance.getOtherThan(*this).getNodeId());

        cout << "TestLoopPerformanceLocalUndeliveredEventActor::onUndeliveredEvent(), localWrittenSize="
                  << localWrittenSize << ", otherWrittenSize=" << otherWrittenSize << endl;

        ASSERT_EQ(0u, outOfBoundWrittenSize);
        ASSERT_EQ(localWrittenSize, sizeof(Event));
        ASSERT_EQ(0u, otherWrittenSize);
    }
};

struct TestLoopPerformanceEventActor: TestLoopPerformanceNeutralActor, simplx::Actor::Callback
{
    TestLoopPerformanceEventActor(PerformanceCounters *performanceCounters)
        : TestLoopPerformanceNeutralActor(*performanceCounters)
    {
        registerEventHandler<Event>(*this);
        registerPerformanceNeutralCallback(*this);
    }
    void onCallback() throw()
    {
        cout << "TestLoopPerformanceEventActor::onCallback()" << endl;
        Event::Pipe(*this, ActorIdSetThreadSafe::instance.getOtherThan(*this)).push<Event>();
    }
    void onEvent(const Event &) throw() { cout << "TestLoopPerformanceEventActor::onEvent()" << endl; }
};

struct TestLoopPerformanceUndeliveredEventActor : TestLoopPerformanceNeutralActor, simplx::Actor::Callback
{
    TestLoopPerformanceUndeliveredEventActor(PerformanceCounters *performanceCounters)
        : TestLoopPerformanceNeutralActor(*performanceCounters)
    {
        registerUndeliveredEventHandler<Event>(*this);
        registerPerformanceNeutralCallback(*this);
    }
    void onCallback() throw()
    {
        cout << "TestLoopPerformanceUndeliveredEventActor::onCallback()" << endl;
        Event::Pipe(*this, TestLoopPerformanceEventActor::ActorIdSetThreadSafe::instance.getOtherThan(*this))
            .push<Event>();
    }
    void onUndeliveredEvent(const Event &) throw()
    {
        uint64_t outOfBoundWrittenSize =
            getCorePerformanceCounters().getTotalWrittenEventByteSizeTo((NodeId)getEngine().getCoreSet().size());
        uint64_t localWrittenSize =
            getCorePerformanceCounters().getTotalWrittenEventByteSizeTo(getActorId().getNodeId());
        uint64_t otherWrittenSize = getCorePerformanceCounters().getTotalWrittenEventByteSizeTo(
            TestLoopPerformanceNeutralActor::ActorIdSetThreadSafe::instance.getOtherThan(*this).getNodeId());

        cout << "TestLoopPerformanceUndeliveredEventActor::onUndeliveredEvent(), localWrittenSize="
                  << localWrittenSize << ", otherWrittenSize=" << otherWrittenSize << endl;

        ASSERT_EQ(0u, outOfBoundWrittenSize);
        ASSERT_EQ(0u, localWrittenSize);
        ASSERT_EQ(otherWrittenSize, sizeof(Event));
    }
};

template <typename _ActorCore1, typename _ActorCore2>
void testLoopPerformanceCounter(
    const TestLoopPerformanceNeutralActor::PerformanceCounters &expectedPerformanceCounters1,
    const TestLoopPerformanceNeutralActor::PerformanceCounters &expectedPerformanceCounters2)
{
    TestLoopPerformanceNeutralActor::PerformanceCounters performanceCounters1;
    TestLoopPerformanceNeutralActor::PerformanceCounters performanceCounters2;
    simplx::Engine::StartSequence startSequence;
    startSequence.addActor<_ActorCore1>(0, &performanceCounters1);
    startSequence.addActor<_ActorCore2>(1, &performanceCounters2);
    {
        simplx::Engine engine(startSequence);
        simplx::Thread::sleep(simplx::Time::Second(1));
    }

    cout << "loopUsageCountCore1=" << performanceCounters1.loopUsageCount
              << ", loopUsageCountCore2=" << performanceCounters2.loopUsageCount << endl;
    cout << "onEventCountCore1=" << performanceCounters1.onEventCount
              << ", onEventCountCore2=" << performanceCounters2.onEventCount << endl;
    cout << "onCallbackCountCore1=" << performanceCounters1.onCallbackCount
              << ", onCallbackCountCore2=" << performanceCounters2.onCallbackCount << endl;

    ASSERT_EQ(expectedPerformanceCounters1, performanceCounters1);
    ASSERT_EQ(expectedPerformanceCounters2, performanceCounters2);
}

void testLoopPerformanceCounter()
{
    testLoopPerformanceCounter<TestLoopPerformanceNeutralCallbackActor, TestLoopPerformanceNeutralCallbackActor>(
        TestLoopPerformanceNeutralActor::PerformanceCounters(0, 0, 0),
        TestLoopPerformanceNeutralActor::PerformanceCounters(0, 0, 0));
    testLoopPerformanceCounter<TestLoopPerformanceCallbackActor, TestLoopPerformanceCallbackActor>(
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 1),
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 1));
    testLoopPerformanceCounter<TestLoopPerformanceDestroyActor, TestLoopPerformanceDestroyActor>(
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 0),
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 0));
    testLoopPerformanceCounter<TestLoopPerformanceLocalEventActor, TestLoopPerformanceLocalEventActor>(
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 1, 0),
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 1, 0));
    testLoopPerformanceCounter<TestLoopPerformanceLocalUndeliveredEventActor,
                               TestLoopPerformanceLocalUndeliveredEventActor>(
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 0),
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 0));
    testLoopPerformanceCounter<TestLoopPerformanceEventActor, TestLoopPerformanceEventActor>(
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 1, 0),
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 1, 0));
    testLoopPerformanceCounter<TestLoopPerformanceUndeliveredEventActor, TestLoopPerformanceNeutralCallbackActor>(
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 0),
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 0));
    testLoopPerformanceCounter<TestLoopPerformanceNeutralCallbackActor, TestLoopPerformanceUndeliveredEventActor>(
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 0),
        TestLoopPerformanceNeutralActor::PerformanceCounters(1, 0, 0));
}

} // namespace anonymous

TEST(Actor, undelivered) { testUndelivered(); }
TEST(Actor, actorReference) { testActorReference(); }
TEST(Actor, actorMultipleReference) { testActorMultipleReference(); }
TEST(Actor, detectionOfEventLoopEnd) { testDetectionOfEventLoopEnd(); }
TEST(Actor, loopPerformanceCounter) { testLoopPerformanceCounter(); }
