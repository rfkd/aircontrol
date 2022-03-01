/*
 * This file is part of aircontrol.
 *
 * Copyright (C) 2014-2022 Ralf Dauberschmidt <ralf@dauberschmidt.de>
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
#include "Types.h"

/// Base class for all task classes.
class Task {
public:
    /// Class constructor.
    Task(Configuration & configuration);

    /// Check if the given GPIO pin is valid.
    static bool isValidGpioPin(const uint8_t gpioPin);

    /// Set the GPIO pin.
    void setGpioPin(const uint8_t gpioPin);

    /// Start the task.
    virtual int start(void) = 0;

protected:
    /// GPIO pin.
    uint8_t gpioPin_ = Types::INVALID_GPIO_PIN;

    /// Reference of the configuration.
    Configuration & configuration_;
};
