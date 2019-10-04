/**
 * @file serialbufferchain.h
 * @brief serialized buffer list (deprecated, replaced by mmap serial buffer)
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#pragma once

// replacement using mmap
#include "simplx_core/internal/mmapserialbuffer.h"

namespace simplx
{
    using SerialBuffer = mmapSerialBuffer;
    
} // namespace simplx
