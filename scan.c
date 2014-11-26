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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <wiringPi.h>

#include <scan.h>
#include <main.h>


bool scanValidate(Scan *scan) {
	if (scan->gpioPin < 0) {
		fprintf(stderr, "Configuration error (scan): gpioPin undefined.\n");
		return false;
	} else  {
		int p;
		unsigned char validPins[] =		// Valid GPIOs on hardware revision 1
			{ 0, 1, 4, 14, 15, 17, 18, 21, 22, 23, 24, 10, 9, 25, 11, 8, 7};

		// Adapt pin list for hardware revision 2
		if (piBoardRev() != 1) {
			validPins[0] = 2;
			validPins[1] = 3;
			validPins[7] = 27;
		}

		for (p = 0; p < sizeof(validPins)
			&& scan->gpioPin != validPins[p]; p++);
		if (p == sizeof(validPins)) {
			fprintf(stderr, "Configuration error (scan): gpioPin (%d) "
				"invalid.\n", scan->gpioPin);
			return false;
		}
	}

	if (scan->samplingRate < 0) {
		fprintf(stderr, "Configuration error (scan): samplingRate "
			"undefined.\n");
		return false;
	} else if (scan->samplingRate == 0) {
		fprintf(stderr, "Configuration error (scan): samplingRate "
			"invalid.\n");
		return false;
	}

	return true;
}

char *scanRaw(Scan *scan) {
	int dataPos = 0, dataSize;
	char *data;

	pinMode(scan->gpioPin, INPUT);

	dataSize = scan->scanLength*1000 / scan->samplingRate;
	data = (char *)malloc(dataSize);
	if (data == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		return NULL;
	}
	bzero(data, dataSize);

	while (dataPos < dataSize) {
		data[dataPos++] = digitalRead(scan->gpioPin) + '0';
		delayMicroseconds(scan->samplingRate);
	}

	return data;
}
