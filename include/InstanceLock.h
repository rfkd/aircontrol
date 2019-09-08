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

/**
 * @brief Class to avoid multiple program instances.
 *
 * A lock file will be created. If another program instance is started in
 * parallel (and the lock file exists) the class will block execution until the
 * lock file has been removed (which is done at program exit automatically).
 * This will prevent accessing shared resources by multiple instances.
 */
class InstanceLock {
public:
    /// Create a lock.
    static void lock(void);

private:
    /// Absolute path of the lock file.
    static const std::string LOCK_FILE;

    /// Release the lock.
    static void unlock(void);
};
