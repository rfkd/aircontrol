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

#include <wiringPi.h>

#include "Scan.h"

/**
 * @param configuration Reference of the configuration.
 * @param durationMs Air scan duration (unit: milliseconds).
 */
Scan::Scan(Configuration & configuration, const int32_t durationMs) :
        Task(configuration),
        durationMs_(durationMs),
        parameters_(nullptr),
        data_() {
    // Do nothing
}

/// @return Program exit code.
int Scan::start(void) {
    assert(parameters_ == nullptr);
    parameters_ = std::make_unique<ScanParameters>(
        ScanParameters(configuration_));

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

    // Perform the air scan and print the results
    airScan();
    printData();

    return EXIT_SUCCESS;
}

void Scan::airScan(void) {
    const int32_t MICROSECONDS_PER_MILLISECOND = 1000;
    const int32_t SAMPLES = (durationMs_ * MICROSECONDS_PER_MILLISECOND)
        / parameters_->getSamplingRate();

    pinMode(gpioPin_, INPUT);

    // Collect the data
    data_.clear();
    while (data_.size() < static_cast<size_t>(SAMPLES)) {
        data_.push_back(digitalRead(gpioPin_) > 0);
        delayMicroseconds(parameters_->getSamplingRate());
    }
}

void Scan::printData(void) const {
    bool previousData = false;

    for (auto i = 0U; i < data_.size(); i++) {
        if (data_.at(i)) {
            if (!previousData) {
                std::cout << "+----+" << std::endl;
            }
            std::cout << "     |" << std::endl;
        } else {
            if (previousData) {
                std::cout << "+----+" << std::endl;
            }
            std::cout << "|" << std::endl;
        }

        previousData = data_.at(i);
    }
}
