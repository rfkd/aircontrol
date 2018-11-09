/*
 * This file is part of aircontrol.
 *
 * Copyright (C) 2014-2018 Ralf Dauberschmidt <ralf@dauberschmidt.de>
 *
 * aircontrol is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * aircontrol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with aircontrol.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

/// Namespace for miscellaneous types.
namespace Types {

/// Supported radio frame encoding types.
struct AirCode {
    /// Supported radio frame encoding types.
    enum AirCode_ {
        MANCHESTER = 0,
        REMOTE_CONTROLLED_OUTLET = 1,
        TORMATIC = 2,
        MAX
    };
};

/// Signature to be used to identify dump files.
static const uint32_t DUMP_SIGNATURE = 0xDEADC0DEU;

/// Invalid GPIO pin marker.
static const uint8_t INVALID_GPIO_PIN = UINT8_MAX;

/// Invalid parameter marker.
static const int32_t INVALID_PARAMETER = INT32_MIN;

} // namespace Types
