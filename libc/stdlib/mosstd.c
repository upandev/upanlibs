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
#include <stdio.h>
#include <mosstd.h>
#include <string.h>
#include <sys/auxv.h>

static process_init_fini_t* _process_init_fini_list = NULL;

extern void load_environ(const char** environ);

__thread int _lib_data1_thread_local = 1000;
__thread int _lib_global1_thread_local;

void access_thread_local_test() {
  _lib_data1_thread_local += getpid();
  _lib_global1_thread_local += 10;
}

typedef struct  {
  uint64_t ti_module;
  uint64_t ti_offset;
} PACKED tls_index;

static uint64_t* THREAD_CONTROL_BLOCK_DTV = (uint64_t*)((uint64_t)THREAD_LOCAL_META_DATA + sizeof(_thread_local_meta_space));

//This is used implicitly by the linker to resolve TLS variable references
//It should not be declared as static to ensure this function appears in the dynamic symbol table of libc.so,
//which then can be used by other shared libraries to resolve and relocate the usage of this function in their code
//while accessing TLS variables
UNUSED uint64_t __tls_get_addr(tls_index* ti) {
  return THREAD_CONTROL_BLOCK_DTV[ti->ti_module] + ti->ti_offset;
}

//this is used inside crt start-up code - called at first before transferring control to main()
UNUSED void _process_init_relocate(int argc, char** argv) {
  _process_init_fini_list = SysProcess_InitRelocate();
  process_init_fini_t* i = _process_init_fini_list;

  //the last entry is main executable
  while (!i->_end) {
    if (i->_init) {
      i->_init();
    }
    i++;
  }

  uint32_t argvSize = sizeof(uintptr_t) * argc;
  int e;
  for(e = 0; e < argc; ++e) {
    argvSize += strlen(argv[e]) + 1;
  }
  load_environ((const char**)((uintptr_t)argv + argvSize));
}

extern void __cxa_finalize(void*);

extern void _stdio_term();
void exit(int rv) {
  /* If we are using stdio, try to shut it down.  At the very least,
   * this will attempt to commit all buffered writes.  It may also
   * unbuffer all writable files, or close them outright.
   * Check the stdio routines for details. */
  //if (_stdio_term)
  if (!isthread()) {
    _stdio_term();
  }

  if (!iskernel() && !isthread()) {
    __cxa_finalize(NULL);

    //the last entry is for the main executable
    process_init_fini_t* i = _process_init_fini_list;
    while (!i->_end) {
      if (i->_fini) {
        i->_fini();
      }
      i++;
    }
  }

  _exit(rv);
}

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

int isthread() {
  return THREAD_LOCAL_META_DATA->_is_thread;
}

int getpid() {
  return THREAD_LOCAL_META_DATA->_pid;
}

void yield() {
  SysProcess_Yield();
}

void waitpid(int pid) {
  SysProcess_WaitPID(pid);
}

void waitonlock(uint64_t lockAddress, int oldVal, int newVal) {
  SysProcess_WaitOnLock(lockAddress, oldVal, newVal);
}

int waitqueue(int id, void* mutex, const struct timeval* timeout) {
  return SysProcess_WaitQueue(id, mutex, timeout);
}

void waitdequeue(int id, bool all) {
  SysProcess_WaitDequeue(id, all);
}

int chdrive(const char* szDriveName) {
  char* retPwd;
  int r = SysDrive_ChangeDrive(szDriveName, &retPwd);
  if (!r) {
    setenv("PWD", retPwd, 1);
  }
  return r;
}

int get_drive_list(DriveStat** pDriveList, int* iListSize) {
  return SysDrive_ShowDrives(pDriveList, iListSize);
}

int mount(const char* szDriveName) {
  return SysDrive_Mount(szDriveName);
}

int umount(const char* szDriveName) {
  return SysDrive_UnMount(szDriveName);
}

int format(const char* szDriveName) {
  return SysDrive_Format(szDriveName);
}

int getcurdrive(DriveStat* pDriveStat) {
  return SysDrive_GetCurrentDriveStat(pDriveStat);
}

int read(int fd, void* buf, int len) {
  return SysFS_FileRead(fd, buf, len) ;
}

int write(int fd, const void* buf, int len) {
  return SysFS_FileWrite(fd, buf, len) ;
}

void select(io_descriptor* waitIODescriptors, io_descriptor* readyIODescriptors) {
  SysFS_FileSelect(waitIODescriptors, readyIODescriptors);
}

int lseek(int fd, int offset, int seekType) {
  return SysFS_FileSeek(fd, offset, seekType) ;
}

unsigned tell(int fd) {
  return SysFS_FileTell(fd) ;
}

int getomode(int fd) {
  return SysFS_FileOpenMode(fd) ;
}

int create(const char* file_path, unsigned short file_attr) {
  return SysFS_CreateFile(file_path, file_attr) ;
}

int open(const char* file_name, uint32_t mode) {
  return SysFS_FileOpen(file_name, mode) ;
}

int openstream(uint32_t mode) {
  return SysFS_FileOpenStream(mode) ;
}

int close(int fd) {
  return SysIO_Close(fd) ;
}

int unlink(const char* filePath) {
  return SysFS_DeleteDirectory(filePath);
}

int mkdir(const char* dirPath, uint16_t attr) {
  return SysFS_CreateDirectory(dirPath, attr);
}

int stat(const char* szFileName, struct stat* pFileStat) {
  return SysFS_FileStat(szFileName, pFileStat) ;
}

int fstat(int iFD, struct stat* pFileStat) {
  return SysFS_FileStatFD(iFD, pFileStat) ;
}

int access(const char* szFileName, int mode) {
  return SysFS_FileAccess(szFileName, mode);
}

int dup2(int oldFD, int newFD) {
  return SysFS_Dup2(oldFD, newFD) ;
}

int getcwd(char* buf, size_t size) {
  return SysFS_CWD(buf, size) ;
}

int chdir(const char* dirPath) {
  char* pwd;
  int r = SysFS_ChangeDirectory(dirPath, &pwd);
  if (!r) {
    setenv("PWD", pwd, 1);
    free(pwd);
  }
  return r;
}

int setsid() {
  return SysProcess_SetSID();
}

uint64_t getauxval(uint64_t type) {
  static const char PLATFORM[] = "x86_64";
  if (type == AT_PLATFORM) {
    return (uint64_t)PLATFORM;
  }
  return 0;
}