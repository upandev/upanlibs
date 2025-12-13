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
# include <fs.h>
# include <sys/select.h>

int SysFS_ChangeDirectory(const char* szDirPath, char** retPwd) {
  return _upanix_syscall(SYS_CALL_CHANGE_DIR, (uint64_t)szDirPath, (uint64_t)retPwd, 3, 4, 5);
}

int SysFS_CWD(char* uiReturnDirPathAddress, int len) {
  return _upanix_syscall(SYS_CALL_CWD, (uint64_t)uiReturnDirPathAddress, (uint64_t)len, 3, 4, 5);
}

int SysFS_CreateDirectory(const char* szDirPath, unsigned short usAttribute) {
  return _upanix_syscall(SYS_CALL_MKDIR, (uint64_t)szDirPath, (uint64_t)usAttribute, 3, 4, 5);
}

int SysFS_DeleteDirectory(const char* szDirPath) {
  return _upanix_syscall(SYS_CALL_RMDIR, (uint64_t)szDirPath, 2, 3, 4, 5);
}

DIR* SysFS_OpenDir(const char* szDirPath) {
  return (DIR*)_upanix_syscall(SYS_CALL_FILE_OPEN_DIR, (uint64_t)szDirPath, 2, 3, 4, 5);
}

struct dirent* SysFS_ReadDir(DIR* dirp) {
  return (struct dirent*)_upanix_syscall(SYS_CALL_FILE_READ_DIR, (uint64_t)dirp, 2, 3, 4, 5);
}

int SysFS_CloseDir(DIR* dirp) {
  return _upanix_syscall(SYS_CALL_FILE_CLOSE_DIR, (uint64_t)dirp, 2, 3, 4, 5);
}

int SysFS_CreateFile(const char* szDirPath, mode_t mode) {
  return _upanix_syscall(SYS_CALL_FILE_CREATE, (uint64_t)szDirPath, (uint64_t)mode, 3, 4, 5);
}

int SysFS_FileOpen(const char* szFileName, int flags, mode_t mode) {
  return _upanix_syscall(SYS_CALL_FILE_OPEN, (uint64_t)szFileName, (uint64_t)flags, (uint64_t)mode, 4, 5);
}

int SysFS_FileOpenStream(uint32_t mode) {
  return _upanix_syscall(SYS_CALL_FILE_OPEN_STREAM, (uint64_t)mode, 2, 3, 4, 5);
}

int SysFS_FileRead(int fd, void* buf, int len) {
  return _upanix_syscall(SYS_CALL_FILE_READ, (uint64_t)fd, (uint64_t)buf, (uint64_t)len, 4, 5);
}

int SysFS_FileWrite(int fd, const void* buf, int len) {
  return _upanix_syscall(SYS_CALL_FILE_WRITE, (uint64_t)fd, (uint64_t)buf, (uint64_t)len, 4, 5);
}

int SysFS_FileSelect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
  return (int)_upanix_syscall(SYS_CALL_FILE_SELECT, (uint64_t)nfds, (uint64_t)readfds, (uint64_t)writefds, (uint64_t)exceptfds, (uint64_t)timeout);
}

int SysFS_FileSeek(int fd, int offSet, int seekType) {
  return _upanix_syscall(SYS_CALL_FILE_SEEK, (uint64_t)fd, (uint64_t)offSet, (uint64_t)seekType, 4, 5);
}

int SysFS_FileTell(int fd) {
  return _upanix_syscall(SYS_CALL_FILE_TELL, (uint64_t)fd, 2, 3, 4, 5);
}

int SysFS_FileOpenMode(int fd) {
  return _upanix_syscall(SYS_CALL_FILE_MODE, (uint64_t)fd, 2, 3, 4, 5);
}

int SysFS_FileStat(const char* szFileName, struct stat* pFileStat) {
  return _upanix_syscall(SYS_CALL_FILE_STAT, (uint64_t)szFileName, (uint64_t)pFileStat, 3, 4, 5);
}

int SysFS_FileStatFD(int iFD, struct stat* pFileStat) {
  return _upanix_syscall(SYS_CALL_FILE_STAT_FD, (uint64_t)iFD, (uint64_t)pFileStat, 3, 4, 5);
}

int SysFS_FileAccess(const char* szFileName, int mode) {
  return _upanix_syscall(SYS_CALL_FILE_ACCESS, (uint64_t)szFileName, (uint64_t)mode, 3, 4, 5);
}

int SysFS_Dup2(int oldFD, int newFD) {
  return _upanix_syscall(SYS_CALL_FILE_DUP2, (uint64_t)oldFD, (uint64_t)newFD, 3, 4, 5);
}