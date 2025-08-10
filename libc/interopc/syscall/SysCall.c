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
# include <syscalldefs.h>

uint64_t _upanix_syscall(uint64_t sysCallId, uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t p5) {
	__volatile__ uint64_t retVal;

	//rsp is adjusted manually because compiler doesn't adjust rsp based on the push instructions in below inline assembly
	//if we add more params to this function, then the rsp adjustment should be updated as needed
  __asm__ __volatile__("sub $128, %%rsp;"
                       "pushq %1;"
                       "pushq %2;"
                       "pushq %3;"
                       "pushq %4;"
                       "pushq %5;"
                       "pushq %6;"
                       "syscall;"
                       "movq %%rax, %0;"
                       "add $48, %%rsp;" //adjust rsp for pushing %1 to %6 params onto the stack
                       "add $128, %%rsp;" //put back stack to original value
                       : "=m"(retVal) : "rm"(p5), "rm"(p4), "rm"(p3), "rm"(p2), "rm"(p1), "rm"(sysCallId) : "memory");
	return retVal;
}