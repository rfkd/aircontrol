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

#include <string>

#include "Configuration.h"

/// Class holding all parameters required for Replay tasks.
class ReplayParameters {
public:
    /// Class constructor.
    ReplayParameters(const Configuration & configuration);

    /**
     * @brief Load all required configuration parameters.
     * @note Must be called before any of the getters.
     */
    bool load(void);

    /// Get the GPIO pin.
    uint8_t getGpioPin(void) const;

private:
    /// Configuration data.
    const Configuration & configuration_;

    /// GPIO pin.
    uint8_t gpioPin_;

    /// Load the GPIO pin from the configuration.
    bool loadGpioPin(void);
};
