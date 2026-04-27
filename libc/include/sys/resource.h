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
#ifndef _SYS_RESOURCE_H_
#define _SYS_RESOURCE_H_

#include <time.h>

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  RUSAGE_SELF = 0,
  RUSAGE_CHILDREN = -1
} RUSAGE_ID;

struct rusage {
  struct timeval ru_utime;
  struct timeval ru_stime;
  long ru_maxrss;
};

int getrusage(RUSAGE_ID who, struct rusage *usage);

#if defined __cplusplus
}
#endif

#endif