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

#include <cassert>
#include <iostream>

#include "ScanParameters.h"
#include "Task.h"
#include "Types.h"

/// @param configuration Reference of the configuration.
ScanParameters::ScanParameters(const Configuration & configuration) :
        configuration_(configuration),
        gpioPin_(Types::INVALID_GPIO_PIN),
        samplingRateUs_(Types::INVALID_PARAMETER) {
    // Do nothing
}

/// @return Status of the operation.
bool ScanParameters::load(void) {
    return loadGpioPin()
        && loadSamplingRate();
}

/// @return GPIO pin.
uint8_t ScanParameters::getGpioPin(void) const {
    assert(gpioPin_ != Types::INVALID_GPIO_PIN);
    return gpioPin_;
}

/// @return Delay between two scan samples.
int32_t ScanParameters::getSamplingRate(void) const {
    assert(samplingRateUs_ != Types::INVALID_PARAMETER);
    return samplingRateUs_;
}

/// @return True if successful, false otherwise.
bool ScanParameters::loadGpioPin(void) {
    int32_t value;

    if (!configuration_.getValue("scan", "gpioPin", value)) {
        std::cerr << "Error: Missing configuration parameter 'gpioPin'."
            << std::endl;
        return false;
    }

    if (!Task::isValidGpioPin(value)) {
        std::cerr << "Error: Configuration error (scan): gpioPin " << value
            << " is invalid" << std::endl;
        return false;
    }

    gpioPin_ = static_cast<uint8_t>(value);

    return true;
}

/// @return True if successful, false otherwise.
bool ScanParameters::loadSamplingRate(void) {
    if (!configuration_.getValue("scan", "samplingRate", samplingRateUs_)) {
        std::cerr << "Error: Missing configuration parameter 'samplingRate'."
            << std::endl;
        return false;
    }

    if (samplingRateUs_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (scan): samplingRate is "
            "undefined" << std::endl;
        return false;
    } else if (samplingRateUs_ <= 0) {
        std::cerr << "Error: Configuration error (scan): samplingRate is "
            "invalid" << std::endl;
        return false;
    }

    return true;
}
