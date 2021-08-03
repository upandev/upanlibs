/*
 *  Mother Operating System - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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

int SysFS_ChangeDirectory(const char* szDirPath)
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

	__asm__ __volatile__("pushl %0" : : "rm"(szDirPath)) ;
	DO_SYS_CALL(SYS_CALL_CHANGE_DIR) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

void SysFS_PWD(char** uiReturnDirPathAddress)
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

	__asm__ __volatile__("pushl %0" : : "rm"(uiReturnDirPathAddress)) ;
	DO_SYS_CALL(SYS_CALL_PWD) ;
	__asm__ __volatile__("pop %eax") ;
}

int SysFS_CWD(char* uiReturnDirPathAddress, int len)
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

	__asm__ __volatile__("pushl %0" : : "rm"((unsigned)len)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(uiReturnDirPathAddress)) ;
	DO_SYS_CALL(SYS_CALL_CWD) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_CreateDirectory(const char* szDirPath, unsigned short usAttribute)
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

	__asm__ __volatile__("pushl %0" : : "rm"((unsigned)usAttribute)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szDirPath)) ;
	DO_SYS_CALL(SYS_CALL_MKDIR) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_DeleteDirectory(const char* szDirPath)
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

	__asm__ __volatile__("pushl %0" : : "rm"(szDirPath)) ;
	DO_SYS_CALL(SYS_CALL_RMDIR) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_GetDirContent(const char* szDirPath, FS_Node** pDirList, int* iListSize)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(iListSize)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(pDirList)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szDirPath)) ;
	DO_SYS_CALL(SYS_CALL_GET_DIR_LIST) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_CreateFile(const char* szDirPath, unsigned short usAttribute)
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

	__asm__ __volatile__("pushl %0" : : "rm"((unsigned)usAttribute)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szDirPath)) ;
	DO_SYS_CALL(SYS_CALL_FILE_CREATE) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileOpen(const char* szFileName, byte bMode)
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

	__asm__ __volatile__("pushl %0" : : "rm"((unsigned)bMode)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szFileName)) ;
	DO_SYS_CALL(SYS_CALL_FILE_OPEN) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileClose(int fd)
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

	__asm__ __volatile__("pushl %0" : : "rm"(fd)) ;
	DO_SYS_CALL(SYS_CALL_FILE_CLOSE) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileRead(int fd, char* buf, int len)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(len)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(buf)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(fd)) ;
	DO_SYS_CALL(SYS_CALL_FILE_READ) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileWrite(int fd, const char* buf, int len)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(len)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(buf)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(fd)) ;
	DO_SYS_CALL(SYS_CALL_FILE_WRITE) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileSeek(int fd, int offSet, int seekType)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(seekType)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(offSet)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(fd)) ;
	DO_SYS_CALL(SYS_CALL_FILE_SEEK) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileTell(int fd)
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

	__asm__ __volatile__("pushl %0" : : "rm"(fd)) ;
	DO_SYS_CALL(SYS_CALL_FILE_TELL) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileOpenMode(int fd)
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

	__asm__ __volatile__("pushl %0" : : "rm"(fd)) ;
	DO_SYS_CALL(SYS_CALL_FILE_MODE) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileStat(const char* szFileName, struct stat* pFileStat)
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

	__asm__ __volatile__("pushl %0" : : "rm"(pFileStat)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szFileName)) ;
	DO_SYS_CALL(SYS_CALL_FILE_STAT) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileStatFD(int iFD, struct stat* pFileStat)
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

	__asm__ __volatile__("pushl %0" : : "rm"(pFileStat)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(iFD)) ;
	DO_SYS_CALL(SYS_CALL_FILE_STAT_FD) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_FileAccess(const char* szFileName, int mode)
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

	__asm__ __volatile__("pushl %0" : : "rm"(mode)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szFileName)) ;
	DO_SYS_CALL(SYS_CALL_FILE_ACCESS) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int SysFS_Dup2(int oldFD, int newFD)
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

	__asm__ __volatile__("pushl %0" : : "rm"(newFD)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(oldFD)) ;
	DO_SYS_CALL(SYS_CALL_FILE_DUP2) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

int read(int fd, char* buf, int len)
{
	return SysFS_FileRead(fd, buf, len) ;
}

int write(int fd, const char* buf, int len)
{
	return SysFS_FileWrite(fd, buf, len) ;
}

int lseek(int fd, int offset, int seekType)
{
	return SysFS_FileSeek(fd, offset, seekType) ;
}

unsigned tell(int fd)
{
	return SysFS_FileTell(fd) ;
}

int getomode(int fd)
{
	return SysFS_FileOpenMode(fd) ;
}

int create(const char* file_path, unsigned short file_attr)
{
	return SysFS_CreateFile(file_path, file_attr) ;
}

int open(const char* file_name, byte mode)
{
	return SysFS_FileOpen(file_name, mode) ;
}

int close(int fd)
{
	return SysFS_FileClose(fd) ;
}

int stat(const char* szFileName, struct stat* pFileStat)
{
	return SysFS_FileStat(szFileName, pFileStat) ;
}

int fstat(int iFD, struct stat* pFileStat)
{
	return SysFS_FileStatFD(iFD, pFileStat) ;
}

int access(const char* szFileName, int mode)
{
	return SysFS_FileAccess(szFileName, mode);
}

int dup2(int oldFD, int newFD)
{
	return SysFS_Dup2(oldFD, newFD) ;
}

int getcwd(char* buf, size_t size)
{
	return SysFS_CWD(buf, size) ;
}

