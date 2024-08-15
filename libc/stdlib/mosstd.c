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

void sleep(uint32_t seconds) {
  SysProcess_Sleep(seconds * 1000);
}

void sleepms(uint32_t milliseconds) {
  SysProcess_Sleep(milliseconds);
}

int getpid() {
  _thread_local_space* tls = (_thread_local_space*)(THREAD_LOCAL_SHARED_ADDRESS);
  return tls->_pid;
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

void waitqueue(int id, void* mutex) {
  SysProcess_WaitQueue(id, mutex);
}

void waitdequeue(int id, bool all) {
  SysProcess_WaitDequeue(id, all);
}

int chdrive(const char* szDriveName) {
  return SysDrive_ChangeDrive(szDriveName);
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
  return SysFS_FileClose(fd) ;
}

int unlink(const char* filePath) {
  return SysFS_DeleteDirectory(filePath);
}

int mkdir(const char* dirPath, uint16_t attr) {
  return SysFS_CreateDirectory(dirPath, attr);
}

int get_dir_content(const char* dirPath, struct stat_ex** dirList, int* size) {
  return SysFS_GetDirContent(dirPath, dirList, size);
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
  return SysFS_ChangeDirectory(dirPath);
}