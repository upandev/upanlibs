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

#include <stdint.h>
#include <stdlib.h>
#include <syscalldefs.h>

static void thread_entry_caller(thread_entry_func_p tmain, void* arg) {
  tmain(arg);
  exit(0);
}

int exect(thread_entry_func_p entryPoint, void* arg) {
  return SysProcess_ThreadExec((uint32_t)thread_entry_caller, (uint32_t)entryPoint, arg);
}

int childalive(int pid) {
  return SysProcess_IsChildAlive(pid) ;
}

int isprocessalive(int pid) {
  return SysProcess_IsProcessAlive(pid);
}

int iskernel() {
  return SysProcess_IsKernel();
}

void sleep(uint32_t seconds) {
  SysProcess_Sleep(seconds * 1000);
}

void sleepms(uint32_t milliseconds) {
  SysProcess_Sleep(milliseconds);
}

int getpid() {
  return SysProcess_GetPID();
}

void yield() {
  SysProcess_Yield();
}