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

#include "Configuration.h"

const std::string Configuration::DEFAULT_LOCATION = "/etc/aircontrol.conf";

/// @param location Absolute configuration file location.
void Configuration::setLocation(const std::string & location) {
    location_ = location;
}

/// @return True if the configuration has been loaded, false otherwise.
bool Configuration::load(void) {
    assert(!isLoaded_);

    try {
        configuration_.readFile(location_.c_str());
    } catch (const libconfig::FileIOException & exception) {
        std::cerr << "Configuration error (" << location_
            << "): file not found" << std::endl;
        return false;
    } catch (const libconfig::ParseException & exception) {
        std::cerr << "Configuration error (" << exception.getFile() << ":"
            << exception.getLine() << "): " << exception.getError()
            << std::endl;
        return false;
    }

    isLoaded_ = true;
    return true;
}

/**
 * @param section Configuration section to be checked.
 * @return True if the given section exists, false otherwise.
 */
bool Configuration::isValidSection(const std::string section) const {
    assert(isLoaded_);

    try {
        configuration_.lookup(section);
        return true;
    } catch (const libconfig::SettingNotFoundException &) {
        return false;
    }
}
