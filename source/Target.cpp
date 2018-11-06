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
#include <unistd.h>

#include <wiringPi.h>

#include "Target.h"

/// @param configuration Reference of the configuration.
Target::Target(Configuration & configuration) : Task(configuration),
        parameters_(nullptr) {
    // Do nothing
}

/**
 *
 * @param name Target name string, must match a target configuration entry.
 * @return Program exit code.
 */
int Target::start(const std::string name) {
    // Check whether the target exists
    if (!configuration_.isValidSection(name)) {
        std::cerr << "Error: Given target " << name << " cannot be found"
            << std::endl;
        return EXIT_FAILURE;
    }

    assert(parameters_ == nullptr);
    parameters_ = std::make_unique<TargetParameters>(
        TargetParameters(configuration_, name));

    // Load all parameters from the configuration
    if (!parameters_->load()) {
        return EXIT_FAILURE;
    }

    // Get GPIO from the parameters unless overridden from the command line
    if (gpioPin_ == Types::INVALID_GPIO_PIN) {
        gpioPin_ = parameters_->getGpioPin();
    } else if (!isValidGpioPin(gpioPin_)) {
        std::cerr << "Error: Given GPIO pin " << +gpioPin_ << " is invalid"
            << std::endl;
        return EXIT_FAILURE;
    }

    // Prepare hardware
    pinMode(gpioPin_, OUTPUT);

    // Control the target
    switch (parameters_->getAirCode()) {
        case Types::AirCode::MANCHESTER:
            controlManchester();
            break;

        case Types::AirCode::REMOTE_CONTROLLED_OUTLET:
            controlRemoteControlledOutled();
            break;

        case Types::AirCode::MAX:
        default:
            assert(false);
            break;
    }

    return EXIT_SUCCESS;
}

void Target::controlManchester(void) const {
    assert(parameters_->getAirCode() == Types::AirCode::MANCHESTER);

    for (int32_t n = 0; n < parameters_->getSendCommand(); n++) {
        digitalWrite(gpioPin_, LOW);

        for (uint32_t i = 0U; i < parameters_->getAirCommand().length(); i++) {
            switch (parameters_->getAirCommand().at(i)) {
                case 's':
                    digitalWrite(gpioPin_, LOW);
                    usleep(parameters_->getSyncLength());
                    break;

                case 'S':
                    digitalWrite(gpioPin_, HIGH);
                    usleep(parameters_->getSyncLength());
                    break;

                case '0':
                    // Falling edge in the middle of the pulse
                    digitalWrite(gpioPin_, HIGH);
                    usleep(parameters_->getDataLength() / 2);
                    digitalWrite(gpioPin_, LOW);
                    usleep(parameters_->getDataLength() / 2);
                    break;

                case '1':
                    // Rising edge in the middle of the pulse
                    digitalWrite(gpioPin_, LOW);
                    usleep(parameters_->getDataLength() / 2);
                    digitalWrite(gpioPin_, HIGH);
                    usleep(parameters_->getDataLength() / 2);
                    break;
            }
        }
        digitalWrite(gpioPin_, LOW);

        if (n != parameters_->getSendCommand() - 1)
            usleep(parameters_->getSendDelay());
    }
}

void Target::controlRemoteControlledOutled(void) const {
    assert(parameters_->getAirCode()
        == Types::AirCode::REMOTE_CONTROLLED_OUTLET);

    for (int32_t n = 0; n < parameters_->getSendCommand(); n++) {
        digitalWrite(gpioPin_, LOW);

        for (uint32_t i = 0U; i < parameters_->getAirCommand().length(); i++) {
            digitalWrite(gpioPin_, HIGH);
            switch (parameters_->getAirCommand().at(i)) {
                case '0':
                    // Falling edge after 25% of the pulse
                    usleep(parameters_->getDataLength() / 4);
                    digitalWrite(gpioPin_, LOW);
                    usleep((parameters_->getDataLength() / 4) * 3);
                    break;

                case '1':
                    // Falling edge after 75% of the pulse
                    usleep((parameters_->getDataLength() / 4) * 3);
                    digitalWrite(gpioPin_, LOW);
                    usleep(parameters_->getDataLength() / 4);
                    break;
            }
        }

        if (n != parameters_->getSendCommand() - 1)
            usleep(parameters_->getSendDelay());
    }
}
