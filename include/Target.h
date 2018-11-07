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

#include <memory>
#include <string>

#include "Configuration.h"
#include "TargetParameters.h"
#include "Task.h"

/// Class responsible for target control.
class Target : public Task {
public:
    /// Class constructor.
    Target(Configuration & configuration, const std::string & name);

    /// Start the target control.
    int start(void) final;

private:
    /// Target section name.
    const std::string name_;

    /// Target parameters.
    std::unique_ptr<TargetParameters> parameters_;

    /// Control a target with Manchester encoding.
    void controlManchester(void) const;

    /// Control a target with RCO encoding.
    void controlRemoteControlledOutled(void) const;
};