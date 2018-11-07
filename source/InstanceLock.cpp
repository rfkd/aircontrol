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

#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "InstanceLock.h"

const std::string InstanceLock::LOCK_FILE = "/tmp/aircontrol.lock";

void InstanceLock::lock(void) {
    struct flock lock;
    int fd;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    // Try to create the lock file
    fd = open(LOCK_FILE.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        std::cerr << "Unable to create lock file (" << LOCK_FILE << ")."
            << std::endl;
        exit(EXIT_FAILURE);
    }

    // Try to acquire the lock
    if (fcntl(fd, F_SETLK, &lock) < 0) {
        std::cout << "Another instance of this program is running, waiting..."
            << std::endl;

        while (fcntl(fd, F_SETLK, &lock) < 0) {
            usleep(100*1000);
        }
    }

    // Ensure the lock gets released upon program termination
    atexit(unlock);
}

void InstanceLock::unlock(void) {
    unlink(LOCK_FILE.c_str());
}
