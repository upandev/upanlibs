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

int SysMemory_Alloc(void** addr, unsigned uiSizeInBytes) {
  return _upanix_syscall(SYS_CALL_ALLOC, (uint64_t)addr, (uint64_t)uiSizeInBytes, 3, 4, 5);
}

int SysMemory_Free(void* uiAddress) {
  return _upanix_syscall(SYS_CALL_FREE, (uint64_t)uiAddress, 2, 3, 4, 5);
}

int SysMemory_GetAllocSize(void* address, size_t* size) {
  return _upanix_syscall(SYS_CALL_GET_ALLOC_SIZE, (uint64_t)address, (uint64_t)size, 3, 4, 5);
}
