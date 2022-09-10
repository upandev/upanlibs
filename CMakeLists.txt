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
cmake_minimum_required(VERSION 3.15)
project(UpanixLibs)

set(CROSS_TOOL_PATH $ENV{HOME}/opt/cross)
set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(CMAKE_CXX_COMPILER ${CROSS_TOOL_PATH}/bin/i686-elf-g++)
set(CMAKE_C_COMPILER ${CROSS_TOOL_PATH}/bin/i686-elf-gcc)
set(CMAKE_LINKER ${CROSS_TOOL_PATH}/i686-elf/bin/ld)
set(CMAKE_AR ${CROSS_TOOL_PATH}/i686-elf/bin/ar)

set(ENABLE_CXX_RT ON)
	
set(EXCEPTION_SUP " -fno-exceptions -fno-rtti")
if(ENABLE_CXX_RT)
	set(EXCEPTION_SUP " -fexceptions -frtti")
endif()

set(SO_COMPILE_FLAGS -shared -fPIC)
set(SO_LD_FLAGS " -Wl,-shared -Wl,-fPIC")

set(CMAKE_CXX_FLAGS " -O0 -Wall -ffreestanding -nodefaultlibs -nostdlib -nostartfiles -nostdinc -std=c++11 -nostdinc++ -fno-threadsafe-statics -fpermissive ${EXCEPTION_SUP}")
set(COMMON_COMPILE_FLAGS " -std=c11 -O0 -Wall -std=c11 -nostdlib -nodefaultlibs -ffreestanding -nostartfiles -D__GCC__")
set(CMAKE_C_FLAGS ${COMMON_COMPILE_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${COMMON_COMPILE_FLAGS} ${SO_LD_FLAGS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${PROJECT_SOURCE_DIR}/libc/include)
include_directories(${PROJECT_SOURCE_DIR}/libc/stdio)
include_directories(${PROJECT_SOURCE_DIR}/libc/sysdeps/mos/common)
include_directories(${PROJECT_SOURCE_DIR}/libc/sysdeps/mos/common/bits)

include_directories(${PROJECT_SOURCE_DIR}/libm/include)
include_directories(${PROJECT_SOURCE_DIR}/libm/include/bits)

include_directories(${PROJECT_SOURCE_DIR}/libcxxrt)

include_directories(${PROJECT_SOURCE_DIR}/libustd/ds)
include_directories(${PROJECT_SOURCE_DIR}/libustd/algo)
include_directories(${PROJECT_SOURCE_DIR}/libustd/infra)
include_directories(${PROJECT_SOURCE_DIR}/libustd/mem)
include_directories(${PROJECT_SOURCE_DIR}/libustd/mthread)

#include_directories(${PROJECT_SOURCE_DIR}/libsfn)
#include_directories(${PROJECT_SOURCE_DIR}/libsfn/freetype2/devel)
#include_directories(${PROJECT_SOURCE_DIR}/libsfn/freetype2/include)
#add_compile_definitions(FT_CONFIG_MODULES_H="<ftmodule.h>")
#add_compile_definitions(FT2_BUILD_LIBRARY)

include_directories(${PROJECT_SOURCE_DIR}/libupanui)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/frame)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uicomponent)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uicomponent/button)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/draw)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/core)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/core/layout)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/image)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/canvas)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/events)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/text)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/text/usfn)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/text/usfn/zlib)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/text/default)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/interop/includes)

add_custom_target(fonts COMMAND make WORKING_DIRECTORY libupanui/text/usfn/fonts)

#LIBC
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libc -name '*.c' | grep -v CMakeFile | grep -v './libc/sys/'" OUTPUT_VARIABLE LIBC_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libc -name '*.c' | grep -v CMakeFile | grep './libc/sys/'" OUTPUT_VARIABLE LIBC_SYS_SRC)
string(REPLACE "\n" ";" LIBC_SRC ${LIBC_SRC})
string(REPLACE "\n" ";" LIBC_SYS_SRC ${LIBC_SYS_SRC})
add_library(c SHARED ${LIBC_SRC} ${LIBC_SYS_SRC})
target_compile_options(c PRIVATE -O0 ${SO_COMPILE_FLAGS})

#LIBC static for kernel - so don't include sys-call code
add_library(upanc STATIC ${LIBC_SRC})
set_target_properties(upanc PROPERTIES OUTPUT_NAME c)

#LIBM
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libm -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE LIBM_SRC)
string(REPLACE "\n" ";" LIBM_SRC ${LIBM_SRC})
#LIBM shared lib
add_library(m SHARED "${LIBM_SRC}")
target_compile_options(m PRIVATE ${SO_COMPILE_FLAGS})
#LIBM static lib for kernel
add_library(upanm STATIC ${LIBM_SRC})
set_target_properties(upanm PROPERTIES OUTPUT_NAME m)

#LIBGCCEH
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libgcceh -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE GCCEH_SRC)
string(REPLACE "\n" ";" GCCEH_SRC ${GCCEH_SRC})
add_library(gcceh STATIC "${GCCEH_SRC}")
target_compile_options(gcceh PRIVATE -DIN_GCC -DIN_LIBGCC2 -Dinhibit_libc -fbuilding-libgcc -fno-stack-protector)

#LIBCXXRT
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libcxxrt -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE CXXRT_SRC1)
string(REPLACE "\n" ";" CXXRT_SRC1 ${CXXRT_SRC1})
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libcxxrt -name '*.cc' | grep -v CMakeFile" OUTPUT_VARIABLE CXXRT_SRC2)
string(REPLACE "\n" ";" CXXRT_SRC2 ${CXXRT_SRC2})
add_library(cxxrt STATIC ${CXXRT_SRC1} ${CXXRT_SRC2})

#LIBUSTD
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libustd -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE LIB_USTD_SRC)
string(REPLACE "\n" ";" LIB_USTD_SRC ${LIB_USTD_SRC})
add_library(ustd STATIC ${LIB_USTD_SRC})

#LIBSFN
#execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libsfn -name '*.c' | grep -v CMakeFile | grep -v builds" OUTPUT_VARIABLE LIB_SFN_SRC)
#string(REPLACE "\n" ";" LIB_SFN_SRC ${LIB_SFN_SRC})
#add_library(sfn STATIC ${LIB_SFN_SRC})

#LIBUPANUI
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ${PROJECT_SOURCE_DIR}/libupanui/text/usfn/fonts/ -name '*.sfn' -exec sh -c 'echo \${1%.sfn}.o' _ {} \;" OUTPUT_VARIABLE FONT_OBJS)
message(${FONT_OBJS})
string(REPLACE "\n" ";" FONT_OBJS ${FONT_OBJS})

execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libupanui -name '*.cpp' | grep -v CMakeFile | grep -v './libupanui/interop/src/'" OUTPUT_VARIABLE LIB_UPANUI_CPP_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libupanui -name '*.cpp' | grep -v CMakeFile | grep './libupanui/interop/src/'" OUTPUT_VARIABLE LIB_UPANUI_INTEROP_CPP_SRC)
string(REPLACE "\n" ";" LIB_UPANUI_CPP_SRC ${LIB_UPANUI_CPP_SRC})
string(REPLACE "\n" ";" LIB_UPANUI_INTEROP_CPP_SRC ${LIB_UPANUI_INTEROP_CPP_SRC})
add_library(upanui STATIC ${FONT_OBJS} ${LIB_UPANUI_CPP_SRC} ${LIB_UPANUI_INTEROP_CPP_SRC})
add_library(upanui_interop STATIC ${FONT_OBJS} ${LIB_UPANUI_CPP_SRC})

add_dependencies(upanui fonts)
add_dependencies(upanui_interop fonts)

link_directories(${OUTPUT_DIR})
