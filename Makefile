#
# This file is part of aircontrol.
# 
# Copyright (C) 2014 Ralf Dauberschmidt <ralf@dauberschmidt.de>
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

CC=gcc
CFLAGS=-O3 -Wall -Wno-unused-result -std=gnu99 -fno-strict-aliasing -I.
LDFLAGS=-lconfig -lwiringPi

SRC=main.c scan.c target.c
OBJ=$(SRC:.c=.o)

all: $(SRC) $(APP)
	
$(APP): $(OBJ) 
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

install: all
	cp $(APP) /usr/local/bin/
	cp $(APP).conf /etc/

uninstall:
	rm -f /usr/local/bin/$(APP)
	rm -f /etc/$(APP).conf

.PHONY: clean
clean:
	rm -f $(APP) $(OBJ)
	
