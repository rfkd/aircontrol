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

#include "Scan.h"

/// @param configuration Reference of the configuration.
Scan::Scan(Configuration & configuration) : Task(configuration) {
    // Do nothing
}

/**
 * @param durationMs Air scan duration (unit: milliseconds).
 * @return Program exit code.
 */
int Scan::start(const int32_t durationMs) {
    //durationMs_ = durationMs;

    std::cout << "Not yet implemented." << std::endl;
    return EXIT_FAILURE;
}
