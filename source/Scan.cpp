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
#include <fstream>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include <wiringPi.h>

#include "Scan.h"

/**
 * @param configuration Reference of the configuration.
 * @param durationMs Air scan duration (unit: milliseconds).
 * @param dumpFile Reference of the dump file. Can be an empty string to dump
 *                 human readable ASCII output to stdout.
 */
Scan::Scan(Configuration & configuration, const int32_t durationMs,
        const std::string & dumpFile) :
        Task(configuration),
        durationMs_(durationMs),
        dumpFile_(dumpFile),
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

    // Perform the air scan and process the results
    airScan();
    if (dumpFile_.length() == 0U) {
        printData();
    } else {
        serializeData();
    }

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
        usleep(parameters_->getSamplingRate());
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

/**
 * Format of the serialized file:
 * - [4 bytes] Signature
 * - [4 bytes] Sampling rate (unit: microseconds)
 * - [n bytes] Sample data, 1 byte each, 0=low / 1=high
 */
void Scan::serializeData(void) const {
    std::ofstream dumpFile;

    assert(dumpFile_.length() > 0U);

    // Open dump file
    dumpFile.open(dumpFile_, std::ios::out | std::ios::binary
        | std::ios::trunc);
    if (!dumpFile.is_open()) {
        std::cerr << "Error: Dump file '" << dumpFile_ << "' cannot be opened "
            "for writing: " << strerror(errno) << std::endl;
        return;
    }

    // Write signature
    if (!dumpFile.write(reinterpret_cast<const char *>(&Types::DUMP_SIGNATURE),
            sizeof(Types::DUMP_SIGNATURE))) {
        std::cerr << "Error: Unable to write signature to dump file: "
            << strerror(errno) << std::endl;
        return;
    }

    // Write sampling rate
    const int32_t samplingRate = parameters_->getSamplingRate();
    if (!dumpFile.write(reinterpret_cast<const char *>(&samplingRate),
            sizeof(samplingRate))) {
        std::cerr << "Error: Unable to write sampling rate to dump file: "
            << strerror(errno) << std::endl;
        return;
    }

    // Write sample data
    for (auto i = 0U; i < data_.size(); i++) {
        const char data = static_cast<char>(data_.at(i));
        if (!dumpFile.write(&data, sizeof(data))) {
            std::cerr << "Error: Unable to write data to dump file: "
                << strerror(errno) << std::endl;
            return;
        }
    }

    // Clean up
    std::cout << "Air scan results dumped successfully to file '" << dumpFile_
        << "'." << std::endl;
}
