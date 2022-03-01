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
#include <fstream>
#include <iostream>
#include <string.h>
#include <unistd.h>

#include <wiringPi.h>

#include "Replay.h"
#include "Types.h"

/**
 * @param configuration Reference of the configuration.
 * @param dumpFile File name of the air Replay dump.
 */
Replay::Replay(Configuration & configuration, const std::string & dumpFile) :
        Task(configuration),
        dumpFile_(dumpFile),
        parameters_(nullptr),
        samplingRateUs_(Types::INVALID_PARAMETER),
        data_() {
    // Do nothing
}

/// @return Program exit code.
int Replay::start(void) {
    assert(parameters_ == nullptr);
    parameters_ = std::make_unique<ReplayParameters>(
        ReplayParameters(configuration_));

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

    // Load the air scan dump
    if (!deserializeData()) {
        return EXIT_FAILURE;
    }

    airReplay();

    return EXIT_SUCCESS;
}

void Replay::airReplay(void) const {
    pinMode(gpioPin_, OUTPUT);

    for (auto i = 0U; i < data_.size(); i++) {
        digitalWrite(gpioPin_, data_.at(i) ? HIGH : LOW);
        usleep(samplingRateUs_);
    }

    pinMode(gpioPin_, INPUT);
}

/**
 * @return Status of the operation.
 *
 * Format of the serialized file:
 * - [4 bytes] Signature
 * - [4 bytes] Sampling rate (unit: microseconds)
 * - [n bytes] Sample data, 1 byte each, 0=low / 1=high
 */
bool Replay::deserializeData(void) {
    std::ifstream dumpFile;

    assert(dumpFile_.length() > 0U);

    // Open dump file
    dumpFile.open(dumpFile_, std::ios::in | std::ios::binary);
    if (!dumpFile.is_open()) {
        std::cerr << "Error: Dump file '" << dumpFile_ << "' cannot be opened "
            "for reading: " << strerror(errno) << std::endl;
        return false;
    }

    // Get the file size
    dumpFile.seekg(0, std::ios::end);
    const size_t dumpFileSize = dumpFile.tellg();
    dumpFile.seekg(0, std::ios::beg);

    // Read signature
    uint32_t signature;
    if (!dumpFile.read(reinterpret_cast<char *>(&signature),
            sizeof(signature))) {
        std::cerr << "Error: Unable to read signature from dump file: "
            << strerror(errno) << std::endl;
        return false;
    }

    // Check signature
    if (signature != Types::DUMP_SIGNATURE) {
        std::cerr << "Error: Given file is not an air scan dump (signature "
            "mismatch)" << std::endl;
        return false;
    }

    // Read sampling rate
    if (!dumpFile.read(reinterpret_cast<char *>(&samplingRateUs_),
            sizeof(samplingRateUs_))) {
        std::cerr << "Error: Unable to read sampling rate from dump file: "
            << strerror(errno) << std::endl;
        return false;
    }

    // Check sampling rate
    if (samplingRateUs_ <= 0) {
        std::cerr << "Error: Given air scan dump seems corrupted (invalid "
            "sampling rate " << samplingRateUs_ << ")" << std::endl;
        return false;
    }

    // Read sample data
    char data;
    while (dumpFile.read(&data, sizeof(data))) {
        if ((data < 0) || (data > 1)) {
            std::cerr << "Error: Given air scan dump seems corrupted (invalid "
                "data value " << +data << ")" << std::endl;
            return false;
        }
        data_.push_back(data == 1);
    }

    // Check sample data
    const size_t expectedDataSize = dumpFileSize - sizeof(signature)
        - sizeof(samplingRateUs_);
    if (data_.size() == 0U) {
        std::cerr << "Error: Given air scan dump seems corrupted (no data "
            "elements found)" << std::endl;
        return false;
    } else if (data_.size() != expectedDataSize) {
        std::cerr << "Error: Given air scan dump seems corrupted (invalid "
            "number of data elements read)" << std::endl;
        return false;
    }

    return true;
}
