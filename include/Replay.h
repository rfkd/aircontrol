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

#include <cstdint>
#include <memory>
#include <vector>

#include "Configuration.h"
#include "ReplayParameters.h"
#include "Task.h"

/// Class responsible for replaying air scan dumps.
class Replay : public Task {
public:
    /// Class constructor.
    Replay(Configuration & configuration, const std::string & dumpFile);

    /// Start replaying the air dump scan.
    int start(void) final;

private:
    /// File name of the air scan dump.
    const std::string dumpFile_;

    /// Replay parameters.
    std::unique_ptr<ReplayParameters> parameters_;

    /**
     * @brief Delay between two scan samples.
     * @note Unit: microseconds
     */
    int32_t samplingRateUs_;

    /**
     * @brief Storage for the air scan dump data. A false element indicates a
     *        low signal, a true element a high signal.
     */
    std::vector<bool> data_;

    /// Perform the air scan replay based on the values stored in 'data_'.
    void airReplay(void) const;

    /// Deserialize the air scan dump data from the dump file.
    bool deserializeData(void);
};
