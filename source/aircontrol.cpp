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

#include <cstdint>
#include <iostream>
#include <unistd.h>

#include <wiringPi.h>

#include "Configuration.h"
#include "InstanceLock.h"
#include "Scan.h"
#include "Target.h"
#include "Task.h"
#include "Version.h"

/// @brief Display the program usage.
static void printUsage(void) {
    std::cout << std::endl
        << "aircontrol " << VERSION << std::endl
        << std::endl
        << "Usage: aircontrol <options>" << std::endl
        << "Available options:" << std::endl
        << "  -c <file>\tConfiguration file [" << Configuration::DEFAULT_LOCATION
        << "]" << std::endl
        << "  -g <pin>\tOverride GPIO pin from configuration" << std::endl
        << "  -l\t\tPrevent multiple program instances" << std::endl
        << "  -s <ms>\tAir scan for given period" << std::endl
        << "  -t <target>\tExecute target configuration" << std::endl
        << std::endl
        << "Copyright (C) 2014-2018 Ralf Dauberschmidt <ralf@dauberschmidt.de>"
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
    Scan scan(configuration);
    int32_t scanDurationMs;
    Target target(configuration);
    std::string targetName;
    enum {
        UNKNOWN,
        SCAN,
        TARGET
    } selectedTask = UNKNOWN;

    // Parse command line arguments
    int option;
    opterr = 0;
    while ((option = getopt(argc, argv, "c:g:ls:t:")) != -1) {
        switch (option) {
            case 'c':
                configuration.setLocation(std::string(optarg));
                break;

            case 'g':
                scan.setGpioPin(static_cast<uint8_t>(atoi(optarg)));
                target.setGpioPin(static_cast<uint8_t>(atoi(optarg)));
                break;

            case 'l':
                InstanceLock::lock();
                break;

            case 's':
                if (atoi(optarg) == 0) {
                    std::cerr << "Error: Air scan duration must be >0ms."
                        << std::endl;
                    return EXIT_FAILURE;
                } else if (selectedTask != UNKNOWN) {
                    std::cerr << "Error: Parallel tasks are not supported "
                        "(omit parameter '-s')." << std::endl;
                    return EXIT_FAILURE;
                }
                scanDurationMs = atoi(optarg);
                selectedTask = SCAN;
                break;

            case 't':
                if (selectedTask != UNKNOWN) {
                    std::cerr << "Error: Parallel tasks are not supported "
                        "(omit parameter '-t')." << std::endl;
                    return EXIT_FAILURE;
                }
                targetName = std::string(optarg);
                selectedTask = TARGET;
                break;

            default:
                printUsage();
                return EXIT_FAILURE;
        }
    }

    // Load the configuration
    if (!configuration.load()) {
        return EXIT_FAILURE;
    }

    // Setup wiringPi: no port re-mapping, use Broadcom GPIO numbers
    wiringPiSetupGpio();

    // Execute the selected task
    switch (selectedTask) {
        case SCAN:
            return scan.start(scanDurationMs);
            break;

        case TARGET:
            return target.start(targetName);
            break;

        default:
            std::cerr << "Error: Either parameter '-s' or '-t' is mandatory."
                << std::endl;
            printUsage();
            return EXIT_FAILURE;
    }
}
