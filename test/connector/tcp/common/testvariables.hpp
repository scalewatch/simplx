/**
 * @author Valerian Vives <valerian.vives@ scalewatch.com>
 * @file testvariables.hpp
 * @brief global variable used by tests
 * @copyright 2019 Scalewatch (www.scalewatch.com).  All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

//#include <simplx.h>

namespace simplx
{
namespace connector
{
namespace tcp
{

std::mutex              g_mutex;
std::condition_variable g_conditionVariable;
bool                    g_finished         = false;
bool                    g_expectingTimeout = false;
bool                    g_failOnTimeout    = false;
} // namespace tcp
} // namespace connector
} // namespace simplx