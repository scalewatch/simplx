/**
 * @file parallel_xplat.cpp
 * @brief parallel implementation all-in-one (for now)
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

// for now same cpp handles both pthread & c++ thread

#include <pthread.h>

#include "simplx_core/internal/parallel/parallel_xplat.h"

#include "simplx_core/internal/linux/platform_linux.h"
#include "simplx_core/internal/rtexception.h"

namespace simplx
{

mutex_t mutexCreate(const MUTEX_T typ)
{
    int cc;
    pthread_mutexattr_t attr;
    
    if ((cc = pthread_mutexattr_init(&attr)) != 0)
    {
        throw RunTimeException(__FILE__, __LINE__, systemErrorToString(cc));
    }
    if ((cc = pthread_mutexattr_settype(&attr, (typ == MUTEX_T::RECURSIVE) ? PTHREAD_MUTEX_RECURSIVE_NP : PTHREAD_MUTEX_FAST_NP)) != 0)
    {
        throw RunTimeException(__FILE__, __LINE__, systemErrorToString(cc));
    }
    mutex_t ret;
    if ((cc = pthread_mutex_init(&ret, &attr)) != 0)
    {
        throw RunTimeException(__FILE__, __LINE__, systemErrorToString(cc));
    }
    return ret;
}

void mutexDestroy(mutex_t &handle)
{
    const int cc = pthread_mutex_destroy(&handle);
    if (cc)     throw RunTimeException(__FILE__, __LINE__, systemErrorToString(cc));
}

void mutexLock(mutex_t &handle)
{
    const int cc = pthread_mutex_lock(&handle);
    if (cc)     throw RunTimeException(__FILE__, __LINE__, systemErrorToString(cc));
}

void mutexUnlock(mutex_t &handle)
{
    const int cc = pthread_mutex_unlock(&handle);
    if (cc)     throw RunTimeException(__FILE__, __LINE__, systemErrorToString(cc));
}

} // namespace simplx

// nada mas
