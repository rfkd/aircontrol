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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#include <libconfig.h>
#include <wiringPi.h>

#include <scan.h>
#include <target.h>
#include <main.h>

// Version token: A
#define VERSION "$Revision: 79 $"


// MISCELLANEOUS ///////////////////////////////////////////////////////////////

/** Display program usage.
 * @param program Program string, typically argv[0].
 */
static void printUsage(const char *program) {
	printf("\naircontrol %s\n\n"
		"Usage: %s [options]\n"
		"Available options:\n"
		"  -c <file>\tConfiguration file [%s]\n"
		"  -g <pin>\tOverride GPIO pin from configuration\n"
		"  -l\t\tPrevent multiple program instances\n"
		"  -s <ms>\tAir scan for given period\n"
		"  -t <target>\tExecute target configuration\n"
		"\nCopyright (C) 2014-2015 Ralf Dauberschmidt "
			"<ralf@dauberschmidt.de>\n\n",
		VERSION, program, AC_CONFIG_FILE);
}

/** Free the instance lock.
 */
static void instanceFree(void) {
	unlink(AC_LOCK_FILE);
}

/** Prevent multiple instances of this program - this function will wait until
 * there are no other instances running.
 */
static void instanceLock(void) {
	struct flock lock;
	int fd;

	fd = open(AC_LOCK_FILE, O_RDWR | O_CREAT, 0600);
	if (fd < 0) {
		fprintf(stderr, "Unable to create lock file (%s).\n", AC_LOCK_FILE);
		exit(RET_ERR_INTERNAL);
	}

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if (fcntl(fd, F_SETLK, &lock) < 0)
		printf("Another instance of this program is running, waiting...\n");
	while (fcntl(fd, F_SETLK, &lock) < 0)
		usleep(100*1000);

	atexit(instanceFree);
}


// MAIN PROGRAM ////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
	int ch, gpioPin = -1;
	char *fileName = NULL, *targetName = NULL;
	unsigned int scanLength = 0;
	bool singleInstance = false;
	config_t cfg;
	config_setting_t *setting;
	Scan scan;
	Target target;

	opterr = 0;
	while ((ch = getopt(argc, argv, "c:g:ls:t:")) != -1) {
		switch (ch) {
			case 'c':
				fileName = strdup(optarg);
				if (fileName == NULL) {
					fprintf(stderr, "Memory allocation failed.\n");
					return RET_ERR_INTERNAL;
				}
				break;

			case 'g':
				gpioPin = atoi(optarg);
				break;

			case 'l':
				singleInstance = true;
				break;

			case 's':
				scanLength = (unsigned int)atoi(optarg);
				break;

			case 't':
				targetName = strdup(optarg);
				if (targetName == NULL) {
					fprintf(stderr, "Memory allocation failed.\n");
					return RET_ERR_INTERNAL;
				}
				break;

			default:
				printUsage(argv[0]);
				return RET_ERR_PARAM;
		}
	}

	if (scanLength == 0 && targetName == NULL) {
		fprintf(stderr, "Either parameter '-s' or '-t' is mandatory.\n");
		return RET_ERR_PARAM;
	}

	if (singleInstance)
		instanceLock();

	if (fileName == NULL) {
		fileName = strdup(AC_CONFIG_FILE);
		if (fileName == NULL) {
			fprintf(stderr, "Memory allocation failed.\n");
			return RET_ERR_INTERNAL;
		}
	}

	config_init(&cfg);
	if (!config_read_file(&cfg, fileName)) {
		fprintf(stderr, "Configuration error (%s:%d): %s.\n",
			config_error_file(&cfg), config_error_line(&cfg),
			config_error_text(&cfg));

		config_destroy(&cfg);
		if (targetName != NULL) free(targetName);
		free(fileName);

		return RET_ERR_CONFIG;
	}

	// No port re-mapping, using Broadcom GPIO numbers
	wiringPiSetupGpio();

	// Air scanning
	if (scanLength > 0) {
		char *scanData;

		scan.gpioPin = gpioPin;
		scan.samplingRate = -1;
		scan.scanLength = scanLength;

		setting = config_lookup(&cfg, "scan");
		if (setting != NULL) {
			if (gpioPin == -1)
				config_setting_lookup_int(setting, "gpioPin", &scan.gpioPin);
			config_setting_lookup_int(setting, "samplingRate",
				&scan.samplingRate);
		}

		if (!scanValidate(&scan)) {
			config_destroy(&cfg);
			if (targetName != NULL) free(targetName);
			free(fileName);

			return RET_ERR_CONFIG;
		}

		scanData = scanRaw(&scan);

		if (scanData != NULL) {
			for (int i = 0, prev = -1; i < strlen(scanData); i++) {
				if (scanData[i] == '1') {
					if (prev == '0') printf("+----+\n");
					printf("     |\n");
				} else {
					if (prev == '1') printf("+----+\n");
					printf("|\n");
				}
				prev = scanData[i];
			}

			free(scanData);
		}

	// Target control
	} else {
		target._name = targetName;
		target.gpioPin = gpioPin;
		target.dataLength = -1;
		target.syncLength = -1;
		target.airCode = -1;
		target.airCommand = NULL;
		target.sendCommand = -1;
		target.sendDelay = -1;

		setting = config_lookup(&cfg, "target");
		if (setting != NULL) {
			if (gpioPin == -1)
				config_setting_lookup_int(setting, "gpioPin", &target.gpioPin);
			config_setting_lookup_int(setting, "dataLength",&target.dataLength);
			config_setting_lookup_int(setting, "syncLength",&target.syncLength);
			config_setting_lookup_int(setting, "airCode", &target.airCode);
			config_setting_lookup_int(setting, "sendCommand",
				&target.sendCommand);
			config_setting_lookup_int(setting, "sendDelay", &target.sendDelay);
		}

		setting = config_lookup(&cfg, targetName);
		if (setting != NULL) {
			if (gpioPin == -1)
				config_setting_lookup_int(setting, "gpioPin", &target.gpioPin);
			config_setting_lookup_int(setting, "dataLength",&target.dataLength);
			config_setting_lookup_int(setting, "syncLength",&target.syncLength);
			config_setting_lookup_int(setting, "airCode", &target.airCode);
			config_setting_lookup_string(setting, "airCommand",
				(const char **)&target.airCommand);
			config_setting_lookup_int(setting, "sendCommand",
				&target.sendCommand);
			config_setting_lookup_int(setting, "sendDelay", &target.sendDelay);
		}

		if (!targetValidate(&target)) {
			config_destroy(&cfg);
			if (targetName != NULL) free(targetName);
			free(fileName);

			return RET_ERR_CONFIG;
		}

		targetControl(&target);
	}

	config_destroy(&cfg);
	free(fileName);
	if (targetName != NULL) free(targetName);

	return RET_OK;
}
