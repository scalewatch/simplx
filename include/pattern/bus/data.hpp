/**
 * @file data.hpp
 * @brief default base data exchanged by thread
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */
#pragma once


namespace xthreadbus {
/**
 * @brief base data type
 * 
 */
class Data
{
    public:
    virtual ~Data() = default;
};

} // namespace xthreadbus