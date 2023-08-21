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
# include <malloc.h>
# include <stdlib.h>
# include <string.h>

extern int SysMemory_Alloc(void** addr, unsigned uiSizeInBytes) ;
extern int SysMemory_Free(void* uiAddress) ;
extern int SysMemory_GetAllocSize(void* address, size_t* size) ;

void* calloc(size_t n, size_t s)
{
	void* a = malloc(n * s) ;
	memset(a, 0, n * s) ;
	return a ;
}

void* malloc(size_t sizeInBytes)
{
  void* addr;
	if(SysMemory_Alloc(&addr, sizeInBytes) < 0)
    return NULL;
  return addr;
}

void free(void* address)
{
	/* If Free fails!!! It should stop / crash the process */
	SysMemory_Free(address) ;
}

int get_alloc_size(void* address, size_t* size)
{
	return SysMemory_GetAllocSize(address, size) ;
}

void* realloc(void* ptr, size_t s)
{
	void* new_ptr = NULL ;

	if(s)
		new_ptr = (void*)malloc(s) ;

	if(ptr)
	{
		size_t old_size ;

		if(get_alloc_size(ptr, &old_size) < 0)
			return (void*)NULL ;

		int copy_size = (old_size < s) ? old_size : s ;

		memcpy(new_ptr, ptr, copy_size) ;
	
		free(ptr) ;
	}

	return new_ptr ;
}
