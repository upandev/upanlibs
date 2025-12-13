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

int SysProcess_Exec(const char* szFileName, const char *const argv[], const char *const envp[]) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_EXEC, (uint64_t)szFileName, (uint64_t)argv, (uint64_t)envp, 4, 5);
}

int SysProcess_ThreadExec(uintptr_t threadCaller, uintptr_t entryAddress, void* arg, bool joinable) {
  return (int)_upanix_syscall(SYS_CALL_THREAD_EXEC, (uint64_t)threadCaller, (uint64_t)entryAddress, (uint64_t)arg, (uint64_t)joinable, 5);
}

int SysProcess_ThreadDetach(int pid) {
  return (int)_upanix_syscall(SYS_CALL_THREAD_DETACH, (uint64_t)pid, 2, 3, 4, 5);
}

int SysProcess_WaitPID(pid_t pid, int *status, int options) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_WAIT_PID, (uint64_t)pid, (uint64_t)status, (uint64_t)options, 4, 5);
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

int SysProcess_Sleep(unsigned milisec) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_SLEEP, (uint64_t)milisec, 2, 3, 4, 5);
}

int SysProcess_GetProcList(PS** pProcList, unsigned* uiListSize) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_GET_PS_LIST, (uint64_t)pProcList, (uint64_t)uiListSize, 3, 4, 5);
}

void SysProcess_FreeProcListMem(PS* pProcList, unsigned uiListSize) {
  _upanix_syscall(SYS_CALL_PROCESS_FREE_PS_LIST, (uint64_t)pProcList, (uint64_t)uiListSize, 3, 4, 5);
}

int SysProcess_MaskSignal(SIG_MASKING_TYPE how, const sigset_t *set, sigset_t *oldset) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_MASK_SIGNAL, (uint64_t)how, (uint64_t)set, (uint64_t)oldset, 4, 5);
}

int SysProcess_SendSignal(pid_t pid, SIGNAL signo, const union sigval* value) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_SIGNAL, (uint64_t)pid, (uint64_t)signo, 3, 4, 5);
}

void SysProcess_SignalReturn(void* signalContext) {
  _upanix_syscall(SYS_CALL_PROCESS_SET_SIGNAL_RETURN, (uint64_t)signalContext, 2, 3, 4, 5);
}

int SysProcess_SetSignalAction(int signo, const struct sigaction *act, struct sigaction *oldact) {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_SET_SIGNAL_ACTION, (uint64_t)signo, (uint64_t)act, (uint64_t)oldact, 4, 5);
}

int SysProcess_SetSID() {
  return (int)_upanix_syscall(SYS_CALL_PROCESS_SET_SID, 1, 2, 3, 4, 5);
}

uint32_t SysProcess_SetAlarm(uint32_t seconds) {
  return (uint32_t)_upanix_syscall(SYS_CALL_PROCESS_SET_ALARM, (uint64_t)seconds, 2, 3, 4, 5);
}

int exec(const char* szFileName, bool hasEnv, const char* arg, va_list argl) {
  const int MAX_ARGS = 128;
  char* argv[MAX_ARGS];
  int argc = 0;
  const char** envp = NULL;

  while (true) {
    if (argc == (MAX_ARGS - 1)) {
      argv[argc] = NULL;
      break;
    }

    const char* p = va_arg(argl, const char*);
    if (p) {
      argv[argc] = (char*) malloc(strlen(p));
      strcpy(argv[argc], p);
    } else {
      argv[argc] = NULL;
      if (hasEnv) {
        envp = va_arg(argl, const char**);
      }
      break;
    }

    ++argc;
  }

  int pid = execve(szFileName, (const char*const*)argv, envp);

  for(int i = 0; i < argc; ++i) {
    free(argv[i]);
  }

  return pid;
}

int execl(const char* szFileName, const char* arg, ...) {
  va_list argl;
  int pid;
  va_start(argl, arg);
  pid = exec(szFileName, false, arg, argl);
  va_end(argl);
  return pid;
}

int execle(const char* szFileName, const char* arg, ...) {
  va_list argl;
  int pid;
  va_start(argl, arg);
  pid = exec(szFileName, true, arg, argl);
  va_end(argl);
  return pid;
}

int execv(const char* szFileName, const char* const argv[]) {
  return execve(szFileName, argv, NULL);
}

int execve(const char* szFileName, const char* const argv[], const char* const envp[]) {
  return SysProcess_Exec(szFileName, argv, envp) ;
}

int SysProcess_IsKernel() {
  return 0;
}

process_init_fini_t* SysProcess_InitRelocate() {
  return (process_init_fini_t*)_upanix_syscall(SYS_CALL_PROCESS_INIT_RELOCATE, 1, 2, 3, 4, 5);
}