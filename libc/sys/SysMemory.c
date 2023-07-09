/*
 *  Mother Operating System - An x86 based Operating System
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

int SysMemory_Alloc(void** addr, unsigned uiSizeInBytes)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %rax") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;

	__asm__ __volatile__("pushq %0" : : "rm"(uiSizeInBytes)) ;
	__asm__ __volatile__("pushq %0" : : "rm"(addr)) ;
	DO_SYS_CALL(SYS_CALL_ALLOC) ;

	__asm__ __volatile__("mov %%rax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %rax") ;
	return iRetStatus;
}

int SysMemory_Free(void* uiAddress)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %rax") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;

	__asm__ __volatile__("pushq %0" : : "rm"((uintptr_t)uiAddress)) ;
	DO_SYS_CALL(SYS_CALL_FREE) ;

	__asm__ __volatile__("mov %%rax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %rax") ;
	return iRetStatus ;
}

int SysMemory_GetAllocSize(void* uiAddress, int* size)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %rax") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;
	__asm__ __volatile__("pushq $0x20") ;

	__asm__ __volatile__("pushq %0" : : "rm"((uintptr_t)size)) ;
	__asm__ __volatile__("pushq %0" : : "rm"((uintptr_t)uiAddress)) ;
	DO_SYS_CALL(SYS_CALL_GET_ALLOC_SIZE) ;

	__asm__ __volatile__("mov %%rax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %rax") ;
	return iRetStatus ;
}
