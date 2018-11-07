#
# This file is part of aircontrol.
# 
# Copyright (C) 2014-2018 Ralf Dauberschmidt <ralf@dauberschmidt.de>
# 
# aircontrol is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# aircontrol is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with aircontrol.  If not, see <http://www.gnu.org/licenses/>.
#

APP=aircontrol

CC=g++
CFLAGS=-std=c++14 -Wall -Wno-unused-result -Iinclude
LDFLAGS=-lconfig -lwiringPi

BIN_DIR=bin
BUILD_DIR=build
ETC_DIR=etc
SRC_DIR=source

SRC:=$(wildcard $(SRC_DIR)/*.cpp)
OBJ:=$(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRC:.cpp=.o))
DEPS:=$(OBJ:.o=.d)

$(BIN_DIR)/$(APP): pre-build scripts/version.sh $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -MMD -MP -MF $(patsubst %.o,%.d,$@) -o $@ $<

.PHONY: pre-build
pre-build:
	@rm -f $(BUILD_DIR)/$(APP).o
	@sh scripts/version.sh

install: all
	cp $(BIN_DIR)/$(APP) /usr/local/bin/
	cp $(ETC_DIR)/$(APP).conf /etc/

uninstall:
	rm -f /usr/local/bin/$(APP)
	rm -f /etc/$(APP).conf

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(APP)

-include $(DEPS)
