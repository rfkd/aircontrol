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

#include "Configuration.h"

const std::string Configuration::DEFAULT_LOCATION = "/etc/aircontrol.conf";

/// @param location Absolute configuration file location.
void Configuration::setLocation(const std::string & location) {
    location_ = location;
}

/// @return True if the configuration has been loaded, false otherwise.
bool Configuration::load(void) {
    assert(!isLoaded_);
    config_init(&configuration_);

    if (config_read_file(&configuration_, location_.c_str()) == CONFIG_FALSE) {
        std::cerr << "Configuration error ("
            << config_error_file(&configuration_)
            << ":" << config_error_line(&configuration_)
            << "): " << config_error_text(&configuration_)
            << "." << std::endl;

        config_destroy(&configuration_);
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
    return config_lookup(&configuration_, section.c_str()) != nullptr;
}

/**
 * @param section Configuration section.
 * @param name Configuration name.
 * @param value Place to store the configuration value to.
 * @return True if the value has been loaded, false otherwise.
 */
bool Configuration::getString(const std::string section,
        const std::string name, std::string & value) const {
    assert(isLoaded_);

    const config_setting_t * const setting =
        config_lookup(&configuration_, section.c_str());
    if (setting == nullptr) {
        return false;
    }

    const char * rawValue = nullptr;
    if ((config_setting_lookup_string(setting, name.c_str(), &rawValue)
            == CONFIG_FALSE) || rawValue == nullptr) {
        return false;
    }
    value = std::string(rawValue);

    return true;
}

/**
 * @param section Configuration section.
 * @param name Configuration name.
 * @param value Place to store the configuration value to.
 * @return True if the value has been loaded, false otherwise.
 */
bool Configuration::getInteger(const std::string section,
        const std::string name, int32_t & value) const {
    assert(isLoaded_);

    const config_setting_t * const setting =
        config_lookup(&configuration_, section.c_str());
    if (setting == nullptr) {
        return false;
    }

    return config_setting_lookup_int(setting, name.c_str(), &value)
        == CONFIG_TRUE;
}
