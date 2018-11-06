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

#include <cassert>
#include <iostream>
#include <string.h>

#include "TargetParameters.h"
#include "Task.h"

/**
 * @param configuration Reference of the configuration.
 * @param name Reference of the related target name.
 */
TargetParameters::TargetParameters(const Configuration & configuration,
        const std::string & name) :
        configuration_(configuration),
        name_(name),
        gpioPin_(Types::INVALID_GPIO_PIN),
        dataLengthUs_(Types::INVALID_PARAMETER),
        syncLengthUs_(Types::INVALID_PARAMETER),
        airCode_(Types::AirCode::MAX),
        airCommand_(),
        sendCommand_(Types::INVALID_PARAMETER),
        sendDelayUs_(Types::INVALID_PARAMETER) {
    // Do nothing
}

/// @return Status of the operation.
bool TargetParameters::load(void) {
    return loadGpioPin()
        && loadDataLength()
        && loadSyncLength()
        && loadAirCode()
        && loadAirCommand()
        && loadSendCommand()
        && loadSendDelay();
}

/// @return GPIO pin.
uint8_t TargetParameters::getGpioPin(void) const {
    assert(gpioPin_ != Types::INVALID_GPIO_PIN);
    return gpioPin_;
}

/// @return Pulse length of a single air command data element (0 or 1).
int32_t TargetParameters::getDataLength(void) const {
    assert(dataLengthUs_ != Types::INVALID_PARAMETER);
    return dataLengthUs_;
}

/// @return Pulse length of a single air command sync element (s or S).
int32_t TargetParameters::getSyncLength(void) const {
    assert(syncLengthUs_ != Types::INVALID_PARAMETER);
    return syncLengthUs_;
}

/// @return Radio frame encoding type to be used.
Types::AirCode::AirCode_ TargetParameters::getAirCode(void) const {
    assert(airCode_ != Types::AirCode::MAX);
    return airCode_;
}

/// @return Sequence string of data and sync elements to be transmitted.
std::string TargetParameters::getAirCommand(void) const {
    assert(airCommand_.length() != 0U);
    return airCommand_;
}

/// @return Number of times the air command will be transmitted.
int32_t TargetParameters::getSendCommand(void) const {
    assert(sendCommand_ != Types::INVALID_PARAMETER);
    return sendCommand_;
}

/// @return Delay between repeated air command transmissions.
int32_t TargetParameters::getSendDelay(void) const {
    assert(sendDelayUs_ != Types::INVALID_PARAMETER);
    return sendDelayUs_;
}

/// @return True if successful, false otherwise.
bool TargetParameters::loadGpioPin(void) {
    int32_t value;

    if (!configuration_.getInteger(name_, "gpioPin", value)) {
        if (!configuration_.getInteger("target", "gpioPin", value)) {
            std::cerr << "Error: Missing configuration parameter 'gpioPin'."
                << std::endl;
            return false;
        }
    }

    if (!Task::isValidGpioPin(value)) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): gpioPin " << +gpioPin_ << " is invalid" << std::endl;
        return false;
    }

    gpioPin_ = static_cast<uint8_t>(value);

    return true;
}

/// @return True if successful, false otherwise.
bool TargetParameters::loadDataLength(void) {
    if (!configuration_.getInteger(name_, "dataLength", dataLengthUs_)) {
        if (!configuration_.getInteger("target", "dataLength", dataLengthUs_)) {
            std::cerr << "Error: Missing configuration parameter 'dataLength'."
                << std::endl;
            return false;
        }
    }

    if (dataLengthUs_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): dataLength is undefined" << std::endl;
        return false;
    } else if (dataLengthUs_ <= 0) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): dataLength is invalid" << std::endl;
        return false;
    }

    return true;
}

/// @return True if successful, false otherwise.
bool TargetParameters::loadSyncLength(void) {
    if (!configuration_.getInteger(name_, "syncLength", syncLengthUs_)) {
        if (!configuration_.getInteger("target", "syncLength", syncLengthUs_)) {
            std::cerr << "Error: Missing configuration parameter 'syncLength'."
                << std::endl;
            return false;
        }
    }

    if (syncLengthUs_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): syncLength is undefined" << std::endl;
        return false;
    } else if (syncLengthUs_ < 0) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): syncLength is invalid" << std::endl;
        return false;
    }

    return true;
}

/// @return True if successful, false otherwise.
bool TargetParameters::loadAirCode(void) {
    int32_t airCode;

    if (!configuration_.getInteger(name_, "airCode", airCode)) {
        if (!configuration_.getInteger("target", "airCode", airCode)) {
            std::cerr << "Error: Missing configuration parameter 'airCode'."
                << std::endl;
            return false;
        }
    }
    airCode_ = static_cast<Types::AirCode::AirCode_>(airCode);

    if (airCode_ >= Types::AirCode::MAX) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): airCode is invalid" << std::endl;
        return false;
    }

    return true;
}

/// @return True if successful, false otherwise.
bool TargetParameters::loadAirCommand(void) {
    if (!configuration_.getString(name_, "airCommand", airCommand_)) {
        if (!configuration_.getString("target", "airCommand", airCommand_)) {
            std::cerr << "Error: Missing configuration parameter 'airCommand'."
                << std::endl;
            return false;
        }
    }

    if (airCommand_.length() == 0U) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): airCommand is undefined" << std::endl;
        return false;
    } else {
        /// @todo Use std::string here.
        const char * availableElements = nullptr;

        switch (airCode_) {
            case Types::AirCode::MANCHESTER:
                availableElements = strdup("01sS");
                break;

            case Types::AirCode::REMOTE_CONTROLLED_OUTLET:
                availableElements = strdup("01");
                break;

            case Types::AirCode::MAX:
            default:
                assert(false);
                break;
        }
        assert(availableElements != nullptr);

        for (uint32_t i = 0U; i < airCommand_.length(); i++) {
            uint32_t j;
            for (j = 0U; (j < strlen(availableElements))
                && (airCommand_.c_str()[i] != availableElements[j]);
                j++);
            if (j == strlen(availableElements)) {
                std::cerr << "Error: Configuration error (target "
                    << name_
                    << "): airCommand contains illegal character at position "
                    << i+1 << std::endl;
                return false;
            }
        }
    }

    return true;
}

/// @return True if successful, false otherwise.
bool TargetParameters::loadSendCommand(void) {
    if (!configuration_.getInteger(name_, "sendCommand",
            sendCommand_)) {
        if (!configuration_.getInteger("target", "sendCommand",
                sendCommand_)) {
            std::cerr << "Error: Missing configuration parameter 'sendCommand'."
                << std::endl;
            return false;
        }
    }

    if (sendCommand_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): sendCommand is undefined" << std::endl;
        return false;
    } else if (sendCommand_ <= 0) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): sendCommand is invalid" << std::endl;
        return false;
    }

    return true;
}

/// @return True if successful, false otherwise.
bool TargetParameters::loadSendDelay(void) {
    if (!configuration_.getInteger(name_, "sendDelay",
            sendDelayUs_)) {
        if (!configuration_.getInteger("target", "sendDelay",
                sendDelayUs_)) {
            std::cerr << "Error: Missing configuration parameter 'sendDelay'."
                << std::endl;
            return false;
        }
    }

    if (sendDelayUs_ == Types::INVALID_PARAMETER) {
        std::cerr << "Error: Configuration error (target " << name_
            << "): sendDelay is undefined" << std::endl;
        return false;
    }

    return true;
}
