/**
 * @file macro.h
 * @brief internally-used C preprocessor macros
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#pragma once

#include <iostream>

/**
 * @def SIMPLX_CORE_DEBUG_BREAK
 * @brief break to debugger upon fatal exception
 */
#ifndef SIMPLX_CORE_DEBUG_BREAK
    // wasn't user-defined
    #ifndef NDEBUG
        // debug
        #define SIMPLX_CORE_DEBUG_BREAK() assert(!true) // break to debugger
    #else
        // release
        #define SIMPLX_CORE_DEBUG_BREAK()
    #endif
#endif

/**
 * @def SIMPLX_TRY
 * @brief Can be used to start a 'try' block
 */
#define SIMPLX_TRY                                                                                                   \
    try                                                                                                                \
    {
/**
 * @def SIMPLX_CATCH_AND_EXIT_FAILURE_WITH_CERR_MESSAGE
 * @brief Will catch all exceptions and exit(EXIT_FAILURE)
 */
#define SIMPLX_CATCH_AND_EXIT_FAILURE_WITH_CERR_MESSAGE                                                              \
    }                                                                                                                  \
    catch (const std::exception &e)                                                                                    \
    {                                                                                                                  \
        std::cerr << e.what() << std::endl;                                                                            \
        exit(EXIT_FAILURE);                                                                                            \
    }                                                                                                                  \
    catch (...)                                                                                                        \
    {                                                                                                                  \
        std::cerr << "Unknown exception" << std::endl;                                                                 \
        exit(EXIT_FAILURE);                                                                                            \
    }

#define SIMPLX_EXPAND_STRING(x) SIMPLX_STRING(x)
/**
 * @def SIMPLX_STRING(x)
 * @brief Convert a define to string
 */
#define SIMPLX_STRING(x) #x

