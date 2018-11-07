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

#include "Configuration.h"
#include "Task.h"
#include "Types.h"

/// Class responsible for air scanning.
class Scan : public Task {
public:
    /// Class constructor.
    Scan(Configuration & configuration, const int32_t durationMs);

    /// Start the air scan.
    int start(void) final;

private:
    /**
     * @brief Air scan duration.
     * @note Unit: milliseconds
     */
    const int32_t durationMs_;

    /**
     * @brief Delay between two samples when air scanning.
     * @note Unit: microseconds
     */
    //int32_t samplingRateUs_ = Types::INVALID_PARAMETER;
};
