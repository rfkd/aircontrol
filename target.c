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
#include <unistd.h>

#include <wiringPi.h>

#include <target.h>
#include <main.h>


bool targetValidate(Target *target) {
	char *airCommandChars;

	if (target->gpioPin < 0) {
		fprintf(stderr, "Configuration error (target %s): gpioPin "
			"undefined.\n", target->_name);
		return false;
	} else {
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
			&& target->gpioPin != validPins[p]; p++);
		if (p == sizeof(validPins)) {
			fprintf(stderr, "Configuration error (target %s): gpioPin (%d) "
				"invalid.\n", target->_name, target->gpioPin);
			return false;
		}
	}

	if (target->dataLength < 0) {
		fprintf(stderr, "Configuration error (target %s): dataLength "
			"undefined.\n", target->_name);
		return false;
	} else if (target->dataLength == 0) {
		fprintf(stderr, "Configuration error (target %s): dataLength "
			"invalid.\n", target->_name);
		return false;
	}

	if (target->syncLength < 0) {
		fprintf(stderr, "Configuration error (target %s): syncLength "
			"undefined.\n", target->_name);
		return false;
	}

	if (target->airCode < 0) {
		fprintf(stderr, "Configuration error (target %s): airCode "
			"undefined.\n", target->_name);
		return false;
	} else if (target->airCode > CODE_WOFI) {
		fprintf(stderr, "Configuration error (target %s): airCode "
			"invalid.\n", target->_name);
		return false;
	}

	if (target->airCommand == NULL) {
		fprintf(stderr, "Configuration error (target %s): airCommand "
			"undefined.\n", target->_name);
		return false;
	}
	if (target->airCode == CODE_MANCHESTER) {
		airCommandChars = strdup("01sS");
		if (airCommandChars == NULL) {
			fprintf(stderr, "Memory allocation failed.\n");
			return RET_ERR_INTERNAL;
		}
	} else if (target->airCode == CODE_RCO) {
		airCommandChars = strdup("01");
		if (airCommandChars == NULL) {
			fprintf(stderr, "Memory allocation failed.\n");
			return RET_ERR_INTERNAL;
		}
	} else if (target->airCode == CODE_TORMATIC) {
		airCommandChars = strdup("01");
		if (airCommandChars == NULL) {
			fprintf(stderr, "Memory allocation failed.\n");
			return RET_ERR_INTERNAL;
		}
	} else if (target->airCode == CODE_WOFI) {
		airCommandChars = strdup("01");
		if (airCommandChars == NULL) {
			fprintf(stderr, "Memory allocation failed.\n");
			return RET_ERR_INTERNAL;
		}
	}
	for (int i = 0; i < strlen(target->airCommand); i++) {
		int c;
		for (c = 0; c < strlen(airCommandChars)
			&& target->airCommand[i] != airCommandChars[c]; c++);
		if (c == strlen(airCommandChars)) {
			fprintf(stderr, "Configuration error (target %s): airCommand "
				"invalid (character '%c' at position %d).\n",
				target->_name, target->airCommand[i],i+1);
			return false;
		}
	}

	if (target->sendCommand < 0) {
		fprintf(stderr, "Configuration error (target %s): sendCommand "
			"undefined.\n", target->_name);
		return false;
	} else if (target->sendCommand == 0) {
		fprintf(stderr, "Configuration error (target %s): sendCommand "
			"invalid.\n", target->_name);
		return false;
	}

	if (target->sendCommand > 1 && target->sendDelay < 0) {
		fprintf(stderr, "Configuration error (target %s): sendDelay "
			"undefined while sendCommand > 1.\n", target->_name);
		return false;
	}

	return true;
}

void targetControl(Target *target) {
	pinMode(target->gpioPin, OUTPUT);

	for (int n = 0; n < target->sendCommand; n++) {
		digitalWrite(target->gpioPin, LOW);

		if (target->airCode == CODE_MANCHESTER) {
			for (int i = 0; i < strlen(target->airCommand); i++) {
				switch (target->airCommand[i]) {
					case 's':
						digitalWrite(target->gpioPin, LOW);
						usleep(target->syncLength);
						break;

					case 'S':
						digitalWrite(target->gpioPin, HIGH);
						usleep(target->syncLength);
						break;

					case '0':
						digitalWrite(target->gpioPin, HIGH);
						usleep(target->dataLength/2);
						digitalWrite(target->gpioPin, LOW);
						usleep(target->dataLength/2);
						break;

					case '1':
						digitalWrite(target->gpioPin, LOW);
						usleep(target->dataLength/2);
						digitalWrite(target->gpioPin, HIGH);
						usleep(target->dataLength/2);
						break;
				}
			}
			digitalWrite(target->gpioPin, LOW);

		} else if (target->airCode == CODE_RCO) {
			for (int i = 0; i < strlen(target->airCommand); i++) {
				digitalWrite(target->gpioPin, HIGH);
				switch (target->airCommand[i]) {
					case '0':
						usleep(target->dataLength/4);
						digitalWrite(target->gpioPin, LOW);
						usleep(target->dataLength/4*3);
						break;

					case '1':
						usleep(target->dataLength/4*3);
						digitalWrite(target->gpioPin, LOW);
						usleep(target->dataLength/4);
						break;
				}
			}
		} else if (target->airCode == CODE_TORMATIC) {
			for (int i = 0; i < strlen(target->airCommand); i++) {
				
				switch (target->airCommand[i]) {
					case '0':
					    digitalWrite(target->gpioPin, HIGH);
						usleep(target->dataLength/3);
						digitalWrite(target->gpioPin, LOW);
						usleep(target->dataLength/3*2);
						printf("0");
						
						break;

					case '1':
					   digitalWrite(target->gpioPin, HIGH);
						usleep(target->dataLength/3);
						digitalWrite(target->gpioPin, LOW);
						usleep(target->dataLength/3);
						digitalWrite(target->gpioPin, HIGH);
						usleep(target->dataLength/3);
						printf("1");
						break;
				}
			}
			
		}
		
		else if (target->airCode == CODE_WOFI) {
			
			usleep(target->sendDelay);
			
			for (int i = 0; i < strlen(target->airCommand); i++) {
				digitalWrite(target->gpioPin, HIGH);
				switch (target->airCommand[i]) {
					case '0':
						
						usleep(target->dataLength/3.8);
						digitalWrite(target->gpioPin, LOW);
						usleep(target->dataLength/1.35);
						break;

					case '1':
						usleep(target->dataLength/1.35);
						digitalWrite(target->gpioPin, LOW);
						usleep(target->dataLength/3.8);
						break;
				}
			}
		} 
		  
		if (n != target->sendCommand-1)
			usleep(target->sendDelay);
	}
}
