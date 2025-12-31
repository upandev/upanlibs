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
#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#include <stdlib.h>

#if defined __cplusplus
extern "C" {
#endif

int waitpid(pid_t pid, int *status, int options);
int wait(int *status);

#define WIFEXITED(s) ((((s) >> 7) & 1) == 1)
#define WEXITSTATUS(s) (((s) >> 8) & 0xFF)
#define WIFSIGNALED(s) (((s) & 0x7F) != 0)
#define WTERMSIG(s) ((s) & 0x7F)
#define WIFSTOPPED(s) ((((s) >> 16) & 0x7F) != 0)
#define WSTOPSIG(s) (((s) >> 16) & 0x7F)
#define WIFCONTINUED(s) ((((s) >> 23) & 1) == 1)

#if defined __cplusplus
}
#endif

#endif