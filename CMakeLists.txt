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

set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

message("Assembler used by CMake: ${CMAKE_ASM_COMPILER}")

set(ENABLE_CXX_RT ON)
	
set(EXCEPTION_SUP " -fno-exceptions -fno-rtti")
if(ENABLE_CXX_RT)
	set(EXCEPTION_SUP " -fexceptions -frtti")
endif()

set(SO_COMPILE_FLAGS -shared -fPIC)
set(SO_LD_FLAGS " -Wl,-shared -Wl,-fPIC")

set(CMAKE_CXX_FLAGS " -O0 -Wall -ffreestanding -nodefaultlibs -nostdlib -nostartfiles -nostdinc -std=c++11 -nostdinc++ -fno-threadsafe-statics -fpermissive ${EXCEPTION_SUP}")
set(COMMON_COMPILE_FLAGS " -O0 -Wall -std=c11 -nostdlib -nodefaultlibs -ffreestanding -nostartfiles -D__GCC__")
set(CMAKE_C_FLAGS ${COMMON_COMPILE_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-T${PROJECT_SOURCE_DIR}/upanso.ld ${COMMON_COMPILE_FLAGS} ${SO_LD_FLAGS}")

add_custom_target(crt
		COMMAND make WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/crt
		BYPRODUCTS ${PROJECT_SOURCE_DIR}/crt/crt0.o ${PROJECT_SOURCE_DIR}/crt/crti.o ${PROJECT_SOURCE_DIR}/crt/crtn.o)

execute_process(COMMAND bash -c "${CMAKE_C_COMPILER} -print-file-name=crtbeginS.o" OUTPUT_VARIABLE CRTBEGIN_SHARED_OBJ)
string(REPLACE "\n" "" CRTBEGIN_SHARED_OBJ ${CRTBEGIN_SHARED_OBJ})
execute_process(COMMAND bash -c "${CMAKE_C_COMPILER} -print-file-name=crtendS.o" OUTPUT_VARIABLE CRTEND_SHARED_OBJ)
string(REPLACE "\n" "" CRTEND_SHARED_OBJ ${CRTEND_SHARED_OBJ})

set(CRTINIT_OBJ "${PROJECT_SOURCE_DIR}/crt/crti.o ${CRTBEGIN_SHARED_OBJ}")
set(CRTFINI_OBJ "${CRTEND_SHARED_OBJ} ${PROJECT_SOURCE_DIR}/crt/crtn.o")

set(CMAKE_C_CREATE_SHARED_LIBRARY "<CMAKE_C_COMPILER> ${CMAKE_C_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS} ${CRTINIT_OBJ} <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${CRTFINI_OBJ}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(${PROJECT_SOURCE_DIR}/libc/include)
include_directories(${PROJECT_SOURCE_DIR}/libc/stdio)
include_directories(${PROJECT_SOURCE_DIR}/libc/sysdeps)
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
include_directories(${PROJECT_SOURCE_DIR}/libustd/net)
include_directories(${PROJECT_SOURCE_DIR}/libustd/stream)
include_directories(${PROJECT_SOURCE_DIR}/libustd/util)

include_directories(${PROJECT_SOURCE_DIR}/libgccucore/i386)
include_directories(${PROJECT_SOURCE_DIR}/libgccucore/zlib)

include_directories(${PROJECT_SOURCE_DIR}/libupanui)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/frame)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uicomponent)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uicomponent/button)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uicomponent/menu)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uicomponent/scroller)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uicomponent/console)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/draw)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/core)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/core/layout)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/image)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/uiobject/canvas)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/events)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/text)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/text/usfn)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/text/default)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/text/textarea)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/image)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/image/bmp)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/image/png)
include_directories(${PROJECT_SOURCE_DIR}/libupanui/interop/includes)

include_directories(${PROJECT_SOURCE_DIR}/libupanssl/include)
include_directories(${PROJECT_SOURCE_DIR}/libupanssl/providers/implementations/include)

add_custom_target(resources COMMAND make WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libupanui/resources/)

#LIBC
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libc -name '*.S' | grep -v CMakeFile | grep -v './libc/interopc/'" OUTPUT_VARIABLE LIBC_ASM_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libc -name '*.[cS]' | grep -v CMakeFile | grep -v './libc/interopc/'" OUTPUT_VARIABLE LIBC_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libc -name '*.c' | grep -v CMakeFile | grep './libc/interopc/'" OUTPUT_VARIABLE LIBC_INTEROPC_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libc -name '*.cpp' | grep -v CMakeFile | grep -v './libc/interopc/'" OUTPUT_VARIABLE LIBCPP_SRC)
string(REPLACE "\n" ";" LIBC_SRC ${LIBC_SRC})
string(REPLACE "\n" ";" LIBCPP_SRC ${LIBCPP_SRC})
string(REPLACE "\n" ";" LIBC_ASM_SRC ${LIBC_ASM_SRC})
string(REPLACE "\n" ";" LIBC_INTEROPC_SRC ${LIBC_INTEROPC_SRC})
set_source_files_properties(${LIBC_ASM_SRC} PROPERTIES LANGUAGE C)
add_library(c SHARED ${LIBC_SRC} ${LIBCPP_SRC} ${LIBC_ASM_SRC} ${LIBC_INTEROPC_SRC})
add_dependencies(c crt)
target_compile_options(c PRIVATE -O0 ${SO_COMPILE_FLAGS})
set_target_properties(c PROPERTIES LINKER_LANGUAGE C)
#LIBC static for kernel - so don't include sys-call code
add_library(upanc STATIC ${LIBC_SRC} ${LIBCPP_SRC} ${LIBC_ASM_SRC})
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
target_compile_options(cxxrt PRIVATE -fPIC)

#LIBUSTD
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libustd -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE LIB_USTD_SRC)
string(REPLACE "\n" ";" LIB_USTD_SRC ${LIB_USTD_SRC})
add_library(ustd STATIC ${LIB_USTD_SRC})

#LIBGCCUCORE
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libgccucore -name '*.cpp' | grep -v CMakeFile" OUTPUT_VARIABLE LIB_GCCUCORE_SRC)
string(REPLACE "\n" ";" LIB_GCCUCORE_SRC ${LIB_GCCUCORE_SRC})
add_library(gccucore STATIC ${LIB_GCCUCORE_SRC})

#LIBUPANUI
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ${PROJECT_SOURCE_DIR}/libupanui/resources/obj -name '*.o';" OUTPUT_VARIABLE RESOURCE_OBJS)
message(${RESOURCE_OBJS})
string(REPLACE "\n" ";" RESOURCE_OBJS ${RESOURCE_OBJS})

execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libupanui -name '*.cpp' | grep -v CMakeFile | grep -v './libupanui/interop/src/'" OUTPUT_VARIABLE LIB_UPANUI_CPP_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libupanui -name '*.cpp' | grep -v CMakeFile | grep './libupanui/interop/src/'" OUTPUT_VARIABLE LIB_UPANUI_INTEROP_CPP_SRC)
string(REPLACE "\n" ";" LIB_UPANUI_CPP_SRC ${LIB_UPANUI_CPP_SRC})
string(REPLACE "\n" ";" LIB_UPANUI_INTEROP_CPP_SRC ${LIB_UPANUI_INTEROP_CPP_SRC})
add_library(upanui STATIC ${RESOURCE_OBJS} ${LIB_UPANUI_CPP_SRC} ${LIB_UPANUI_INTEROP_CPP_SRC})
add_library(upanui_interop STATIC ${RESOURCE_OBJS} ${LIB_UPANUI_CPP_SRC})

add_dependencies(upanui resources)
add_dependencies(upanui_interop resources)

#LIBUPANSSL
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libupanssl/crypto -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE LIBUPANSSL_CRYPTO_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libupanssl/crypto -name '*.S' | grep -v CMakeFile | grep x86_64" OUTPUT_VARIABLE LIBUPANSSL_CRYPTO_ASM_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libupanssl/providers -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE LIBUPANSSL_PROVIDERS_SRC)
execute_process(COMMAND bash -c "cd ${PROJECT_SOURCE_DIR}; find ./libupanssl/ssl -name '*.c' | grep -v CMakeFile" OUTPUT_VARIABLE LIBUPANSSL_SSL_SRC)
string(REPLACE "\n" ";" LIBUPANSSL_CRYPTO_SRC ${LIBUPANSSL_CRYPTO_SRC})
string(REPLACE "\n" ";" LIBUPANSSL_CRYPTO_ASM_SRC ${LIBUPANSSL_CRYPTO_ASM_SRC})
string(REPLACE "\n" ";" LIBUPANSSL_PROVIDERS_SRC ${LIBUPANSSL_PROVIDERS_SRC})
string(REPLACE "\n" ";" LIBUPANSSL_SSL_SRC ${LIBUPANSSL_SSL_SRC})
set_source_files_properties(${LIBUPANSSL_CRYPTO_ASM_SRC} PROPERTIES LANGUAGE C)
#add_library(crypto STATIC ${LIBUPANSSL_CRYPTO_SRC} ${LIBUPANSSL_CRYPTO_ASM_SRC} ${LIBUPANSSL_PROVIDERS_SRC})
#add_library(ssl STATIC ${LIBUPANSSL_SSL_SRC})
#target_link_libraries(ssl PRIVATE crypto)

link_directories(${OUTPUT_DIR})

