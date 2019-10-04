/**
 * @file simplx.h
 * @brief Simplx top-level bulk header
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */


#include "simplx_core/actor.h"
#include "simplx_core/engine.h"
#include "simplx_core/engineversion.h"
#include "simplx_core/initializer.h"
#include "simplx_core/platform.h"

// internal
#include "simplx_core/internal/cacheline.h"
#include "simplx_core/internal/mdoublechain.h"
#include "simplx_core/internal/mforwardchain.h"
#include "simplx_core/internal/serialbufferchain.h"
#include "simplx_core/internal/property.h"
#include "simplx_core/internal/rtexception.h"
#include "simplx_core/internal/stringstream.h"
#include "simplx_core/internal/thread.h"
#include "simplx_core/internal/time.h"
#include "simplx_core/internal/intrinsics.h"
#include "simplx_core/internal/macro.h"

/*
#include "simplx_core/internal/dlldecoration.h"
#include "simplx_core/internal/dlldecorationrestore.h"
#include "simplx_core/internal/dlldecorationsave.h"
*/

//Historical backward compatibility
namespace tredzone = ::simplx;

// [To be release OSS]
// #if SIMPLX_E2E
//     #include "simplx_core/e2e/e2e.h"
// #endif