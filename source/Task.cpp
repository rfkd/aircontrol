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

#include <wiringPi.h>

#include "Task.h"

/// @param configuration Reference of the configuration.
Task::Task(Configuration & configuration) :
        configuration_(configuration) {
    // Do nothing
}

/**
 * @param gpioPin GPIO pin to be checked.
 * @return True if the given GPIO pin is valid, false otherwise.
 */
bool Task::isValidGpioPin(const uint8_t gpioPin) {
    if (gpioPin == Types::INVALID_GPIO_PIN) {
        return false;
    }

    const uint8_t VALID_GPIO_PINS_HW_REV1[] = { 0U, 1U, 4U, 14U, 15U, 17U,
        18U, 21U, 22U, 23U, 24U, 10U, 9U, 25U, 11U, 8U, 7U };
    const uint8_t VALID_GPIO_PINS_HW_REV2[] = { 2U, 3U, 4U, 14U, 15U, 17U,
        18U, 27U, 22U, 23U, 24U, 10U, 9U, 25U, 11U, 8U, 7U };
    const uint8_t * validGpioPins;
    uint8_t VALID_GPIO_PINS_COUNT;

    if (piBoardRev() == 1) {
        validGpioPins = VALID_GPIO_PINS_HW_REV1;
        VALID_GPIO_PINS_COUNT = sizeof(VALID_GPIO_PINS_HW_REV1);
    } else {
        validGpioPins = VALID_GPIO_PINS_HW_REV2;
        VALID_GPIO_PINS_COUNT = sizeof(VALID_GPIO_PINS_HW_REV2);
    }

    uint8_t pin;
    for (pin = 0U; (pin < VALID_GPIO_PINS_COUNT)
        && (gpioPin != validGpioPins[pin]); pin++);
    return pin < VALID_GPIO_PINS_COUNT;
}

/// @param gpioPin GPIO pin.
void Task::setGpioPin(const uint8_t gpioPin) {
    gpioPin_ = gpioPin;
}
