/*
 * This file is part of aircontrol.
 *
 * Copyright (C) 2014-2019 Ralf Dauberschmidt <ralf@dauberschmidt.de>
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

#include <string>

#include "Configuration.h"

/// Class holding all parameters required for Scan tasks.
class ScanParameters {
public:
    /// Class constructor.
    ScanParameters(const Configuration & configuration);

    /**
     * @brief Load all required configuration parameters.
     * @note Must be called before any of the getters.
     */
    bool load(void);

    /// Get the GPIO pin.
    uint8_t getGpioPin(void) const;

    /**
     * @brief Get the delay between two scan samples.
     * @note Unit: microseconds
     */
    int32_t getSamplingRate(void) const;

private:
    /// Reference of the related configuration instance.
    const Configuration & configuration_;

    /// GPIO pin.
    uint8_t gpioPin_;

    /**
     * @brief Delay between two scan samples.
     * @note Unit: microseconds
     */
    int32_t samplingRateUs_;

    /// Load the GPIO pin from the configuration.
    bool loadGpioPin(void);

    /// Load the sampling rate parameter from the configuration.
    bool loadSamplingRate(void);
};
