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
#ifndef _FS_H_
#define _FS_H_

#include <stdlib.h>
#include <mosstd.h>
#include <sys/stat.h>

#define S_IFIFO   0x1000
#define S_IFCHR   0x2000
#define S_IFDIR		0x4000
#define S_IFREG		0x8000
#define S_IFLNK   0xA000
#define S_IFSOCK  0xC000

#define ATTR_DIR_DEFAULT	(0755 | S_IFDIR)  //0000 0001 1110 1101 => 0000(Rsv) 000(Dir) 111(u:rwx) 101(g:r-x) 101(o:r-x)
#define ATTR_FILE_DEFAULT	(0644 | S_IFREG) //0000(Rsv) 001(File) 110(u:rw-) 100(g:r--) 100(o:r--)
#define ATTR_DELETED_DIR	0x1000

#define ATTR_READ	0x4
#define ATTR_WRITE	0x2
#define ATTR_EXE	0x1

#define S_OWNER(perm)		((perm & 0x7) << 6)
#define S_GROUP(perm)		((perm & 0x7) << 3)
#define S_OTHERS(perm)		(perm & 0x7)

#define G_OWNER(perm)		((perm >> 6) & 0x7)
#define G_GROUP(perm)		((perm >> 3) & 0x7)
#define G_OTHERS(perm)		(perm & 0x7)

#define FILE_PERM_MASK	0xFFF
#define FILE_TYPE_MASK	0xF000

#define HAS_READ_PERM(perm)		((perm & 0x7) & ATTR_READ)
#define HAS_WRITE_PERM(perm)	((perm & 0x7) & ATTR_WRITE)
#define HAS_EXE_PERM(perm)		((perm & 0x7) & ATTR_EXE)

#define FILE_PERM(attr)	(attr & FILE_PERM_MASK)
#define FILE_TYPE(attr) (attr & FILE_TYPE_MASK)

#define S_ISDIR(attr) (FILE_TYPE(attr) == S_IFDIR)
#define S_ISREG(attr) (FILE_TYPE(attr) == S_IFREG)
#define S_ISSOCK(attr) (FILE_TYPE(attr) == S_IFSOCK)
#define S_ISCHR(attr) (FILE_TYPE(attr) == S_IFCHR)
#define S_ISLNK(attr) (FILE_TYPE(attr) == S_IFLNK)
#define S_ISFIFO(attr) (FILE_TYPE(attr) == S_IFIFO)

#define	SEEK_SET 0
#define	SEEK_CUR 1
#define	SEEK_END 2

#if defined __cplusplus
extern "C" {
#endif

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 4
#define O_APPEND 8
#define O_CREAT 16
#define O_EXCL 32
#define O_TRUNC 64
#define O_NONBLOCK 128
#define O_NOCTTY 256
#define O_RD_NONBLOCK 512
#define O_WR_NONBLOCK 1024

typedef enum {
  F_OK = 0,   // existence check
  X_OK = 1,   // executable
  W_OK = 2,   // writable
  R_OK = 4,   // readable
} AccessModes;

int create(const char* file_path, mode_t mode) ;
int open(const char* file_name, int flags, ...) ;
int openstream(uint32_t mode);
int close(int fd) ;
int unlink(const char* filePath);

int mkdir(const char* dirPath, uint16_t attr);

int read(int fd, void* buf, int len) ;
int write(int fd, const void* buf, int len) ;
int lseek(int fd, int offset, int seekType) ;
unsigned tell(int fd) ;
int getomode(int fd) ;
int tofileaccessmode(int flags);
int access(const char* szFileName, int mode) ;
int dup(int oldFD);
int dup2(int oldFD, int newFD) ;
int getcwd(char* buf, size_t size);
int chdir(const char* dirPath);
int rename(const char* oldPath, const char* newPath);
int symlink(const char *target, const char *link);
int readlink(const char *link, char *buf, size_t bufsize);

#if defined __cplusplus
}
#endif


#endif
