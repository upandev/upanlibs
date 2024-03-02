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

	__asm__ __volatile__("push %rax");
	__asm__ __volatile__("pushq $0" : : "rm"(p5));
  __asm__ __volatile__("pushq $0" : : "rm"(p4));
  __asm__ __volatile__("pushq $0" : : "rm"(p3));
  __asm__ __volatile__("pushq $0" : : "rm"(p2));
  __asm__ __volatile__("pushq $0" : : "rm"(p1));
  __asm__ __volatile__("pushq $0" : : "rm"(sysCallId));
  __asm__ __volatile__("syscall");
  __asm__ __volatile__("mov %%rax, %0" : "=m"(retVal) : );
  __asm__ __volatile__("add $48, %rsp");
	__asm__ __volatile__("pop %rax");

	return retVal;
}