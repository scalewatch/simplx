/**
 * @file e2e_stub.cpp
 * @brief engine-to-engine stub/dummies for linking (should never get here)
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

// don't use stubs if has actual e2e back-end

#if (!defined(SIMPLX_E2E) || (SIMPLX_E2E==0))

#include "simplx_core/internal/node.h"

using namespace std;

namespace simplx
{

void* Actor::Event::Pipe::newOutOfProcessSharedMemoryEvent(size_t, EventChain *&, uintptr_t, Event::route_offset_type &)
{
    breakThrow(FeatureNotImplementedException());
    return nullptr;
}

void* Actor::Event::Pipe::newOutOfProcessEvent(void *, size_t, EventChain *&, uintptr_t, Event::route_offset_type &)
{
    breakThrow(FeatureNotImplementedException());
    return nullptr;
}

void* Actor::Event::Pipe::newOutOfProcessEvent(size_t sz, EventChain *&destinationEventChain, uintptr_t eventOffset, Event::route_offset_type &routeOffset)
{
    return newOutOfProcessEvent(eventFactory.context, sz, destinationEventChain, eventOffset, routeOffset);
}

void* Actor::Event::Pipe::allocateOutOfProcessSharedMemoryEvent(size_t, void *, uint32_t &, size_t &)
{
    breakThrow(FeatureNotImplementedException());
    return nullptr;
}

void* Actor::Event::Pipe::allocateOutOfProcessSharedMemoryEvent(size_t, void *)
{
    breakThrow(FeatureNotImplementedException());
    return 0;
}

#ifndef NDEBUG
    uint64_t Actor::Event::Pipe::batchOutOfProcessSharedMemoryEvent(void *, bool) noexcept
    {
        SIMPLX_CORE_DEBUG_BREAK();
        return 0;
    }
#else
    uint64_t Actor::Event::Pipe::batchOutOfProcessSharedMemoryEvent(void *) noexcept
    {
        SIMPLX_CORE_DEBUG_BREAK()
        return 0;
    }
#endif

void AsyncNodesHandle::WriterSharedHandle::onUndeliveredEvent(const Actor::Event &event) noexcept
{
    // cl1 = cacheline 1
    assert(cl1.writerNodeHandle);
    assert(cl1.writerNodeHandle->node);
    
    // event cannot be e2e-routed since we're in single-engine configuration
    assert(!event.isRouteToSource());
    
    onUndeliveredEventToSourceActor(event);
}

bool AsyncNodesHandle::WriterSharedHandle::localOnOutboundEvent(const Actor::Event &) noexcept
{
    SIMPLX_CORE_DEBUG_BREAK();      // should never happen
    
    // error
    return false;
}

} // namespace simplx

#endif // SIMPLX_E2E

// nada mas
