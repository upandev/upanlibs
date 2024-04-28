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
#ifndef _UPANIX_STD_H_
#define _UPANIX_STD_H_

#include <stdint.h>
#include <stdlib.h>

#define pid_t int

#define THREAD_LOCAL_SHARED_ADDRESS (512UL * 1024 * 1024 * 1024)

typedef struct {
  pid_t _pid;
} PACKED _thread_local_space;

#if defined __cplusplus
extern "C" {
#endif

typedef enum
{
	NEW,
	RUN,
	WAIT_SLEEP,
	WAIT_INT,
  WAIT_INT_WITH_TIMEOUT,
	WAIT_CHILD,
  WAIT_LOCK,
	WAIT_RESOURCE,
	WAIT_KERNEL_SERVICE,
  WAIT_EVENT,
  WAIT_IO_DESCRIPTORS,
	TERMINATED,
	RELEASED,
  TOTAL_PROC_STATUS
} PROCESS_STATUS ;

typedef enum {
  IO_Read,
  IO_Write
} IO_OP_TYPES ;

typedef struct {
  int _fd;
  IO_OP_TYPES _ioType;
} io_descriptor;

typedef struct
{
	int pid ;
	char* pname ;
	PROCESS_STATUS status ;
	int iParentProcessID ;
	int iProcessGroupID ;
	int iUserID ;
} PS ;

typedef void (*thread_entry_func_p)(void*);

int exec(const char* szFileName, ...) ;
int execv(const char* szFileName, int iNoOfArgs, const char *const szArgList[]) ;
int execvp(const char* szFileName, const char *const szArgList[]) ;
int exect(thread_entry_func_p entryPoint, void* arg);
int childalive(int pid) ;
int isprocessalive(int pid);
int iskernel();
int getpid();
void yield();
void waitonlock(uint64_t lockAddress, int oldVal, int newVal);

extern void SysProcess_WaitPID(int iProcessID) ;
extern void SysProcess_Exit(int iExitStatus) ;
extern int SysProcess_GetProcList(PS** pProcList, unsigned* uiListSize) ;
extern void SysProcess_FreeProcListMem(PS* pProcList, unsigned uiListSize) ;

#define waitpid(pid) SysProcess_WaitPID(pid)
#define _exit(exit_status) SysProcess_Exit(exit_status)
#define getpslist(pslist, size) SysProcess_GetProcList(pslist, size)
#define freepslist(pslist, size) SysProcess_FreeProcListMem(pslist, size)

#if defined __cplusplus
}
#endif

#endif
