/*
 * This file is part of aircontrol.
 *
 * Copyright (C) 2014-2015 Ralf Dauberschmidt <ralf@dauberschmidt.de>
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

#ifndef __MAIN_H
#define __MAIN_H

// Configuration defaults
#define AC_CONFIG_FILE	"/etc/aircontrol.conf"
#define AC_LOCK_FILE	"/tmp/aircontrol.lock"

// Supported codes
enum {
	CODE_MANCHESTER,
	CODE_RCO
};

// Return values
enum {
	RET_OK,
	RET_ERR_PARAM,
	RET_ERR_CONFIG,
	RET_ERR_INTERNAL
};

#endif
