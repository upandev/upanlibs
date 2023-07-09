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
  return SysProcess_ThreadExec((uintptr_t)thread_entry_caller, (uintptr_t)entryPoint, arg);
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

void optimized_memcpy(uint32_t dest, uint32_t src, int len) {
  const int inc = 16 * 8; // number of bytes copied per iteration = 16 bytes per xmm register * 8 xmm registers
  for(int i = 0; i < len; i += inc) {
    __asm__ __volatile__ (
        "prefetchnta 128(%0);"
        "prefetchnta 160(%0);"
        "prefetchnta 192(%0);"
        "prefetchnta 224(%0);"
        "movdqa 0(%0), %%xmm0;"
        "movdqa 16(%0), %%xmm1;"
        "movdqa 32(%0), %%xmm2;"
        "movdqa 48(%0), %%xmm3;"
        "movdqa 64(%0), %%xmm4;"
        "movdqa 80(%0), %%xmm5;"
        "movdqa 96(%0), %%xmm6;"
        "movdqa 112(%0), %%xmm7;"
        "movntdq %%xmm0, 0(%1);"
        "movntdq %%xmm1, 16(%1);"
        "movntdq %%xmm2, 32(%1);"
        "movntdq %%xmm3, 48(%1);"
        "movntdq %%xmm4, 64(%1);"
        "movntdq %%xmm5, 80(%1);"
        "movntdq %%xmm6, 96(%1);"
        "movntdq %%xmm7, 112(%1);"
        : : "r"(src), "r"(dest) : "memory");
    src += inc;
    dest += inc;
  }
}