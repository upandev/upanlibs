/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
 *
 *  I am making my contributions/submissions to this project solely in
 *  my personal capacity and am not conveying any rights to any
 *  intellectual property of any third parties.
 *                                                                          
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                          
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                          
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */
#pragma once

typedef unsigned tcflag_t ;
typedef unsigned char cc_t ;

#define NCCS 20

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  ECHO = 0x8,
  ECHONL = 0x10,
  ISIG = 0x80,
  ICANON = 0x100,
} termios_lflags;

typedef enum {
	ICRNL = 0x100,
	INLCR = 0x40,
	IGNCR = 0x80,
} termios_iflags;

typedef enum {
  ONLCR = 0x2,
} termios_oflags;

typedef enum {
  CS8 = 0x300,
} termios_cflags;

typedef enum {
  TCSANOW,
  TCSADRAIN,
  TCSAFLUSH
} termios_actions;

struct termios
{
	tcflag_t c_iflag ; // input modes
	tcflag_t c_oflag ; // output modes
	tcflag_t c_cflag ; // control modes
	tcflag_t c_lflag ; // local modes
	cc_t c_cc[NCCS] ; // control chars
} ;

int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, termios_actions action, const struct termios *termios_p);

#if defined __cplusplus
}
#endif
