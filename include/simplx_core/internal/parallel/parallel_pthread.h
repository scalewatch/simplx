/**
 * @file parallel_pthread.h
 * @brief parallel operations as pthread implementation
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */
 
#pragma once

#include <pthread.h>

namespace simplx
{

using mutex_t = pthread_mutex_t;
using thread_t = pthread_t;

void mutexDestroy(mutex_t &);
void mutexLock(mutex_t &);
void mutexUnlock(mutex_t &);



} // namespace simplx




 