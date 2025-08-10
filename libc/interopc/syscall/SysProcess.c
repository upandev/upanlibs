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
# include <malloc.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>

int SysProcess_Exec(const char* szFileName, int iNoOfArgs, const char *const szArgList[]) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_EXEC, (uint64_t)szFileName, (uint64_t)iNoOfArgs, (uint64_t)szArgList, 4, 5);
}

int SysProcess_ThreadExec(uintptr_t threadCaller, uintptr_t entryAddress, void* arg) {
  return (int)_upanix_syscall(SYS_CALL_THREAD_EXEC, (uint64_t)threadCaller, (uint64_t)entryAddress, (uint64_t)arg, 4, 5);
}

void SysProcess_WaitPID(int iProcessID) {
  _upanix_syscall(SYS_CALL_PROCESS_WAIT_PID, (uint64_t)iProcessID, 2, 3, 4, 5);
}

void SysProcess_WaitOnLock(uint64_t lockAddress, int oldVal, int newVal) {
  _upanix_syscall(SYS_CALL_PROCESS_WAIT_ON_LOCK, lockAddress, oldVal, newVal, 4, 5);
}

int SysProcess_WaitQueue(int id, void* mutex, const struct timeval* timeout) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_WAIT_QUEUE, id, (uint64_t)mutex, (uint64_t)timeout, 4, 5);
}

void SysProcess_WaitDequeue(int id, bool all) {
  _upanix_syscall(SYS_CALL_PROCESS_WAIT_DEQUEUE, id, (uint64_t)all, 3, 4, 5);
}

int SysProcess_IsChildAlive(int iProcessID) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_CHILD_ALIVE, (uint64_t)iProcessID, 2, 3, 4, 5);
}

int SysProcess_IsProcessAlive(int iProcessID) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_ALIVE, (uint64_t)iProcessID, 2, 3, 4, 5);
}

void SysProcess_Exit(int iExitStatus) {
  _upanix_syscall(SYS_CALL_PROCESS_EXIT, (uint64_t)iExitStatus, 2, 3, 4, 5);
}

void SysProcess_Yield() {
  _upanix_syscall(SYS_CALL_PROCESS_YIELD, 1, 2, 3, 4, 5);
}

void SysProcess_Sleep(unsigned milisec) {
  _upanix_syscall(SYS_CALL_PROCESS_SLEEP, (uint64_t)milisec, 2, 3, 4, 5);
}

int SysProcess_GetEnv(const char* szVar, char* retVal) {
  return _upanix_syscall(SYS_CALL_PROCESS_GET_ENV, (uint64_t)szVar, (uint64_t)retVal, 3, 4, 5);
}

int SysProcess_SetEnv(const char* szVar, const char* szVal) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_SET_ENV, (uint64_t)szVar, (uint64_t)szVal, 3, 4, 5);
}

int SysProcess_GetProcList(PS** pProcList, unsigned* uiListSize) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_GET_PS_LIST, (uint64_t)pProcList, (uint64_t)uiListSize, 3, 4, 5);
}

void SysProcess_FreeProcListMem(PS* pProcList, unsigned uiListSize) {
  _upanix_syscall(SYS_CALL_PROCESS_FREE_PS_LIST, (uint64_t)pProcList, (uint64_t)uiListSize, 3, 4, 5);
}

int SysProcess_Kill(int pid, int signal) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_KILL, (uint64_t)pid, (uint64_t)signal, 3, 4, 5);
}

int exec(const char* szFileName, ...)
{
  int iProcessID ;
  int argc ;
	char** argv = NULL ;

	int i ;
	uintptr_t* ref = (uintptr_t*)&szFileName + 1 ;
	for(argc = 0; *(ref + argc); argc++) ;

	if(argc)
	{
		argv = (char**)malloc(sizeof(char**) * argc) ;
		if(!argv)
			return -1 ;

		for(i = 0; i < argc; i++)
		{
			argv[i] = (char*)malloc(strlen((char*)(*(ref + i))) + 1) ;
			strcpy(argv[i], (const char*)(*(ref + i))) ;
		}
	}

	iProcessID = SysProcess_Exec(szFileName, argc, (const char**const)argv) ;

	for(i = 0; i < argc; i++)
		free((void*)argv[i]) ;
	free(argv) ;

	return iProcessID ;
}

int execv(const char* szFileName, int iNoOfArgs, const char *const szArgList[])
{
	return SysProcess_Exec(szFileName, iNoOfArgs, szArgList) ;
}

int execvp(const char* szFileName, const char *const szArgList[])
{
	__volatile__ int argc ;
	__volatile__ const int max_args = 256 ;


	for(argc = 0; szArgList[ argc ] != NULL; argc++)
	{
		if(argc > max_args)
		{
			printf("\n Number of arguments to execvp exceeded max args of %d", max_args) ;
			return -1 ;
		}
	}

	return SysProcess_Exec(szFileName, argc, szArgList) ;
}

int SysProcess_IsKernel() {
  return 0;
}

void SysProcess_DLLInitRelocate() {
  _upanix_syscall(SYS_CALL_DLL_INIT_RELOCATE, 1, 2, 3, 4, 5);
}