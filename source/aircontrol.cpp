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

#include <cstdint>
#include <iostream>
#include <memory>
#include <unistd.h>

#include <wiringPi.h>

#include "Configuration.h"
#include "InstanceLock.h"
#include "Replay.h"
#include "Scan.h"
#include "Target.h"
#include "Task.h"
#include "Types.h"
#include "Version.h"

/// @brief Display the program usage.
static void printUsage(void) {
    std::cout << std::endl
        << "aircontrol " << VERSION << std::endl
        << std::endl
        << "Usage: aircontrol [options] <command>" << std::endl
        << std::endl
        << "Available options:" << std::endl
        << "  -c <file>\tConfiguration file ["
        << Configuration::DEFAULT_LOCATION << "]" << std::endl
        << "  -d <file>\tDump air scan results to file" << std::endl
        << "  -g <pin>\tOverride GPIO pin from configuration" << std::endl
        << "  -l\t\tPrevent multiple program instances" << std::endl
        << std::endl
        << "Available commands:" << std::endl
        << "  -r <file>\tReplay given air scan dump" << std::endl
        << "  -s <ms>\tAir scan for given period" << std::endl
        << "  -t <target>\tExecute target configuration" << std::endl
        << std::endl
        << "Copyright (C) 2014-2022 Ralf Dauberschmidt <ralf@dauberschmidt.de>"
        << std::endl << std::endl;
}

/**
 * @brief Main entry point.
 * @param argc Number of elements in argv.
 * @param argv Program name and arguments.
 * @return Exit code.
 */
int main(int argc, char **argv) {
    Configuration configuration;
    std::unique_ptr<Task> task;
    uint8_t gpio = Types::INVALID_GPIO_PIN;
    std::string dumpFile;

    // Parse command line arguments
    int option;
    opterr = 0;
    while ((option = getopt(argc, argv, "c:d:g:lr:s:t:")) != -1) {
        switch (option) {
            case 'c':
                configuration.setLocation(std::string(optarg));
                break;

            case 'd':
                if (task != nullptr) {
                    std::cerr << "Error: Parameter '-d' is an option and must "
                        "be placed before the command" << std::endl;
                    return EXIT_FAILURE;
                }
                dumpFile = std::string(optarg);
                break;

            case 'g':
                gpio = static_cast<uint8_t>(atoi(optarg));
                break;

            case 'l':
                InstanceLock::lock();
                break;

            case 'r':
                if (task != nullptr) {
                    std::cerr << "Error: Multiple commands are not supported "
                        "(maybe omit parameter '-r')" << std::endl;
                    return EXIT_FAILURE;
                }
                task = std::make_unique<Replay>(Replay(configuration,
                    std::string(optarg)));
                break;

            case 's':
                if (atoi(optarg) == 0) {
                    std::cerr << "Error: Air scan duration must be >0ms"
                        << std::endl;
                    return EXIT_FAILURE;
                } else if (task != nullptr) {
                    std::cerr << "Error: Multiple commands are not supported "
                        "(maybe omit parameter '-s')" << std::endl;
                    return EXIT_FAILURE;
                }
                task = std::make_unique<Scan>(Scan(configuration,
                    atoi(optarg), dumpFile));
                break;

            case 't':
                if (task != nullptr) {
                    std::cerr << "Error: Multiple commands are not supported "
                        "(maybe omit parameter '-t')" << std::endl;
                    return EXIT_FAILURE;
                }
                task = std::make_unique<Target>(Target(configuration,
                    std::string(optarg)));
                break;

            default:
                printUsage();
                return EXIT_FAILURE;
        }
    }
    if (task == nullptr) {
        std::cerr << "Error: Either parameter '-r', '-s' or '-t' is mandatory"
            << std::endl;
        printUsage();
        return EXIT_FAILURE;
    }

    // Load the configuration
    if (!configuration.load()) {
        return EXIT_FAILURE;
    }

    // Setup wiringPi (no port re-mapping, use Broadcom GPIO numbers)
    task->setGpioPin(gpio);
    wiringPiSetupGpio();

    return task->start();
}
