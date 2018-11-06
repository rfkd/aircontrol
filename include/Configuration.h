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

#pragma once

#include <cstdint>
#include <string>

#include <libconfig.h>

/// Class managing the program configuration.
class Configuration {
public:
    /// Default configuration file location.
    static const std::string DEFAULT_LOCATION;

    /// Set the absolute configuration file location.
    void setLocation(const std::string & location);

    /// Load the configuration file.
    bool load(void);

    /// Check whether the given section exists.
    bool isValidSection(const std::string section) const;

    /// Get the given string value.
    bool getString(const std::string section, const std::string name,
        std::string & value) const;

    /// Get the given integer value.
    bool getInteger(const std::string section, const std::string name,
        int32_t & value) const;

private:
    /// Absolute configuration file location.
    std::string location_ = DEFAULT_LOCATION;

    /// Flag to determine whether the configuration has been loaded.
    bool isLoaded_ = false;

    /// Configuration data.
    config_t configuration_;
};
