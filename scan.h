/*
 * This file is part of aircontrol.
 *
 * Copyright (C) 2014 Ralf Dauberschmidt <ralf@dauberschmidt.de>
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

#ifndef __SCAN_H
#define __SCAN_H

#include <stdbool.h>

typedef struct {
	// Settings from the configuration file
	int		gpioPin;
	int		samplingRate;	// Unit: us

	// Settings from the command line
	int		scanLength;		// Unit: ms
} Scan;


/** Validate the given scan configuration.
 * @param scan Scan configuration to validate.
 * @return True if the configuration is valid, false otherwise.
 */
bool scanValidate(Scan *scan);

/** Perform an air scan and return the raw data.
 * @param scan Scan parameters.
 * @return Raw ASCII-encoded stream of variable length, '0' is low, '1' high.
 */
char *scanRaw(Scan *scan);

#endif
