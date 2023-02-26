#	 Upanix - An x86 based Operating System
#	 Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
#
#  I am making my contributions/submissions to this project solely in
#  my personal capacity and am not conveying any rights to any
#  intellectual property of any third parties.
#																			 
#	 This program is free software: you can redistribute it and/or modify
#	 it under the terms of the GNU General Public License as published by
#	 the Free Software Foundation, either version 3 of the License, or
#	 (at your option) any later version.
#																			 
#	 This program is distributed in the hope that it will be useful,
#	 but WITHOUT ANY WARRANTY; without even the implied warranty of
#	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	 GNU General Public License for more details.
#																			 
#	 You should have received a copy of the GNU General Public License
#	 along with this program.  If not, see <http://www.gnu.org/licenses/
LD = ${HOME}/opt/cross/i686-elf/bin/ld

# List all the font files
FONT_FILES := $(shell find . -name '*.sfn')

# Generate the list of all the output .o files
OBJ_FILES := $(patsubst %.sfn,obj/%.o,$(FONT_FILES))

# Default target to build all the .o files
all: $(OBJ_FILES)

# Rule to build a .o file from a .sfn file
obj/%.o: %.sfn
	mkdir -p $(@D)
	${LD} -r -b binary -o $@ $<

#readelf -s ${FONT_FREE_SANS_OBJ}

clean:
	rm -rf obj/

distclean:
	rm -rf obj/
