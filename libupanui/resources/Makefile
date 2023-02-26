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

# List all the resource files
PNG_FILES := $(shell find icons/png -name '*.png')
BMP_FILES := $(shell find icons/bmp -name '*.bmp')
FONT_FILES := $(shell find fonts/sfn -name '*.sfn')

# Generate the list of all the output .o files
PNG_OBJ_FILES := $(patsubst %.png,obj/%.o,$(PNG_FILES))
BMP_OBJ_FILES := $(patsubst %.bmp,obj/%.o,$(BMP_FILES))
FONT_OBJ_FILES := $(patsubst %.sfn,obj/%.o,$(FONT_FILES))

# Default target to build all the .o files
all: $(PNG_OBJ_FILES) $(BMP_OBJ_FILES) $(FONT_OBJ_FILES)

# Rule to build a .o file from a .png file
obj/%.o: %.png
	mkdir -p $(@D)
	${LD} -r -b binary -o $@ $<

# Rule to build a .o file from a .bmp file
obj/%.o: %.bmp
	mkdir -p $(@D)
	${LD} -r -b binary -o $@ $<

# Rule to build a .o file from a .sfn file
obj/%.o: %.sfn
	mkdir -p $(@D)
	${LD} -r -b binary -o $@ $<

clean:
	rm -rf obj/

distclean:
	rm -rf obj/
