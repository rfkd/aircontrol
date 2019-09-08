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

#include <cassert>
#include <iostream>

#include "ReplayParameters.h"
#include "Task.h"
#include "Types.h"

/// @param configuration Configuration data.
ReplayParameters::ReplayParameters(const Configuration & configuration) :
        configuration_(configuration),
        gpioPin_(Types::INVALID_GPIO_PIN) {
    // Do nothing
}

/// @return Status of the operation.
bool ReplayParameters::load(void) {
    return loadGpioPin();
}

/// @return GPIO pin.
uint8_t ReplayParameters::getGpioPin(void) const {
    assert(gpioPin_ != Types::INVALID_GPIO_PIN);
    return gpioPin_;
}

/// @return True if successful, false otherwise.
bool ReplayParameters::loadGpioPin(void) {
    int32_t value;

    if (!configuration_.getValue("replay", "gpioPin", value)) {
        std::cerr << "Error: Missing configuration parameter 'gpioPin'"
            << std::endl;
        return false;
    }

    if (!Task::isValidGpioPin(value)) {
        std::cerr << "Error: Configuration error (replay): gpioPin " << value
            << " is invalid" << std::endl;
        return false;
    }

    gpioPin_ = static_cast<uint8_t>(value);

    return true;
}
