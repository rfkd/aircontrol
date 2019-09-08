/*
 * This file is part of aircontrol.
 *
 * Copyright (C) 2014-2019 Ralf Dauberschmidt <ralf@dauberschmidt.de>
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

#include <string>

#include "Configuration.h"
#include "Types.h"

/// Class holding all parameters required for Target tasks.
class TargetParameters {
public:
    /// Class constructor.
    TargetParameters(const Configuration & configuration,
        const std::string & name);

    /**
     * @brief Load all required configuration parameters.
     * @note Must be called before any of the getters.
     */
    bool load(void);

    /// Get the GPIO pin.
    uint8_t getGpioPin(void) const;

    /**
     * @brief Get the pulse length of a single air command data element.
     * @note Unit: microseconds
     */
    int32_t getDataLength(void) const;

    /**
     * @brief Get the pulse length of a single air command sync element.
     * @note Unit: microseconds
     */
    int32_t getSyncLength(void) const;

    /// Get the radio frame encoding type.
    Types::AirCode::AirCode_ getAirCode(void) const;

    /// Get the sequence string of data and sync elements to be transmitted.
    std::string getAirCommand(void) const;

    /// Get the number of times the air command will be transmitted.
    int32_t getSendCommand(void) const;

    /**
     * @brief Get the delay between repeated air command transmissions.
     * @note Unit: microseconds
     */
    int32_t getSendDelay(void) const;

private:
    /// Reference of the related configuration instance.
    const Configuration & configuration_;

    /// Reference of the target name.
    const std::string name_;

    /// GPIO pin.
    uint8_t gpioPin_;

    /**
     * @brief Pulse length of a single air command data element (0 or 1).
     * @note Unit: microseconds
     */
    int32_t dataLengthUs_;

    /**
     * @brief Pulse length of a single air command sync element (s or S).
     * @note Unit: microseconds
     */
    int32_t syncLengthUs_;

    /// Radio frame encoding type.
    Types::AirCode::AirCode_ airCode_;

    /// Sequence string of data and sync elements to be transmitted.
    std::string airCommand_;

    /// Number of times the air command will be transmitted.
    int32_t sendCommand_;

    /**
     * @brief Delay between repeated air command transmissions.
     * @note Unit: microseconds
     */
    int32_t sendDelayUs_;

    /**
     * @brief Get the requested configuration value from either the given
     *        section or the "target" section.
     * @copydoc Configuration::getValue()
     */
    template <typename T>
    bool getValue(const std::string section, const std::string name,
            T & value) const {
        if (!configuration_.getValue(section, name, value)) {
            if (!configuration_.getValue("target", name, value)) {
                std::cerr << "Error: Missing configuration parameter '" << name
                    << "'" << std::endl;
                return false;
            }
        }

        return true;
    }

    /// Load the GPIO pin from the configuration.
    bool loadGpioPin(void);

    /// Load the data length parameter from the configuration.
    bool loadDataLength(void);

    /// Load the sync length parameter from the configuration.
    bool loadSyncLength(void);

    /// Load the air code parameter from the configuration.
    bool loadAirCode(void);

    /// Load the air command parameter from the configuration.
    bool loadAirCommand(void);

    /// Load the send command parameter from the configuration.
    bool loadSendCommand(void);

    /// Load the send delay parameter from the configuration.
    bool loadSendDelay(void);
};
