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

#ifndef __TARGET_H
#define __TARGET_H

#include <stdbool.h>

typedef struct {
	// Internal settings
	char *	_name;

	// Settings from the configuration file
	int		gpioPin;
	int		dataLength;		// Unit: us
	int		syncLength;		// Unit: us
	int		airCode;
	char *	airCommand;
	int		sendCommand;
	int		sendDelay;		// Unit: us
} Target;


/** Validate the given target configuration.
 * @param target Target configuration to validate.
 * @return True if the configuration is valid, false otherwise.
 */
bool targetValidate(Target *target);

/** Control the target according to the given configuration.
 * @param target Target configuration to use.
 */
void targetControl(Target *target);

#endif
