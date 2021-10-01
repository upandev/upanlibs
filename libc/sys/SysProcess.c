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

int SysProcess_Exec(const char* szFileName, int iNoOfArgs, const char *const szArgList[])
{
	__volatile__ int iProcessID ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(szArgList)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(iNoOfArgs)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szFileName)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_EXEC) ;
	__asm__ __volatile__("movl %%eax, %0" : "=m"(iProcessID) : ) ;
	__asm__ __volatile__("pop %eax") ;

	return iProcessID ;
}

int SysProcess_ThreadExec(uint32_t threadCaller, uint32_t entryAddress, void* arg)
{
  __volatile__ int threadID;

  __asm__ __volatile__("push %eax") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;

  __asm__ __volatile__("pushl %0" : : "rm"(arg)) ;
  __asm__ __volatile__("pushl %0" : : "rm"(entryAddress)) ;
  __asm__ __volatile__("pushl %0" : : "rm"(threadCaller)) ;
  DO_SYS_CALL(SYS_CALL_THREAD_EXEC) ;
  __asm__ __volatile__("movl %%eax, %0" : "=m"(threadID) : ) ;
  __asm__ __volatile__("pop %eax") ;

  return threadID;
}

void SysProcess_WaitPID(int iProcessID)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(iProcessID)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_WAIT_PID) ;
	__asm__ __volatile__("pop %eax") ;
}

int SysProcess_IsChildAlive(int iProcessID)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(iProcessID)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_CHILD_ALIVE) ;
	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;

	return iRetStatus ;
}

int SysProcess_IsProcessAlive(int iProcessID) {
  __volatile__ int iRetStatus ;

  __asm__ __volatile__("push %eax") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;

  __asm__ __volatile__("pushl %0" : : "rm"(iProcessID)) ;
  DO_SYS_CALL(SYS_CALL_PROCESS_ALIVE) ;
  __asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
  __asm__ __volatile__("pop %eax") ;

  return iRetStatus ;
}

void SysProcess_Exit(int iExitStatus)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(iExitStatus)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_EXIT) ;
	__asm__ __volatile__("pop %eax") ;
}

void SysProcess_Yield()
{
  __asm__ __volatile__("push %eax") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;

  DO_SYS_CALL(SYS_CALL_PROCESS_YIELD) ;
  __asm__ __volatile__("pop %eax") ;
}

void SysProcess_Sleep(unsigned milisec)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(milisec)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_SLEEP) ;
	__asm__ __volatile__("pop %eax") ;
}

int SysProcess_GetPID()
{
	__volatile__ int iProcessID ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	DO_SYS_CALL(SYS_CALL_PROCESS_PID) ;
	__asm__ __volatile__("movl %%eax, %0" : "=m"(iProcessID) : ) ;
	__asm__ __volatile__("pop %eax") ;

	return iProcessID ;
}

const char* SysProcess_GetEnv(const char* szVar)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl %0" : : "rm"(szVar)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_GET_ENV) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return (const char*)iRetStatus ;
}

int SysProcess_SetEnv(const char* szVar, const char* szVal)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(szVal)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szVar)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_SET_ENV) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysProcess_GetProcList(PS** pProcList, unsigned* uiListSize)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(uiListSize)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(pProcList)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_GET_PS_LIST) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

void SysProcess_FreeProcListMem(PS* pProcList, unsigned uiListSize)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(uiListSize)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(pProcList)) ;
	DO_SYS_CALL(SYS_CALL_PROCESS_FREE_PS_LIST) ;
	__asm__ __volatile__("pop %eax") ;
}

int exec(const char* szFileName, ...)
{
	__volatile__ int iProcessID ;
	__volatile__ int argc ;
	char** argv = NULL ;

	__volatile__ int i ;
	__volatile__ int* ref = (int*)&szFileName + 1 ;
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