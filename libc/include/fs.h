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

#define ATTR_DIR_DEFAULT	0x01ED  //0000 0001 1110 1101 => 0000(Rsv) 000(Dir) 111(u:rwx) 101(g:r-x) 101(o:r-x)
#define ATTR_FILE_DEFAULT	0x03A4  //0000(Rsv) 001(File) 110(u:rw-) 100(g:r--) 100(o:r--)
//#define ATTR_FILE_DEFAULT	0x02A4  //0000(Rsv) 001(File) 110(u:rw-) 100(g:r--) 100(o:r--)
#define ATTR_DELETED_DIR	0x1000
#define ATTR_TYPE_DIRECTORY	0x2000
#define ATTR_TYPE_FILE		0x4000

#define ATTR_READ	0x4
#define ATTR_WRITE	0x2
#define ATTR_EXE	0x1

#define S_OWNER(perm)		((perm & 0x7) << 6)
#define S_GROUP(perm)		((perm & 0x7) << 3)
#define S_OTHERS(perm)		(perm & 0x7)

#define G_OWNER(perm)		((perm >> 6) & 0x7)
#define G_GROUP(perm)		((perm >> 3) & 0x7)
#define G_OTHERS(perm)		(perm & 0x7)

#define FILE_PERM_MASK	0x1FF
#define FILE_TYPE_MASK	0xF000

#define HAS_READ_PERM(perm)		((perm & 0x7) & ATTR_READ)
#define HAS_WRITE_PERM(perm)	((perm & 0x7) & ATTR_WRITE)
#define HAS_EXE_PERM(perm)		((perm & 0x7) & ATTR_EXE)

#define FILE_PERM(attr)	(attr & FILE_PERM_MASK)
#define FILE_TYPE(attr) (attr & FILE_TYPE_MASK)

#define S_ISDIR(attr) (FILE_TYPE(attr) == ATTR_TYPE_DIRECTORY)
#define S_ISFILE(attr) (FILE_TYPE(attr) == ATTR_TYPE_FILE)

#define	SEEK_SET 0
#define	SEEK_CUR 1
#define	SEEK_END 2

#if defined __cplusplus
extern "C" {
#endif

typedef enum
{	
	O_RDONLY = 1,
	O_WRONLY = 2,
	O_RDWR = 4,
	O_APPEND = 8,
	O_CREAT = 16,
	O_EXCL = 32,
	O_TRUNC = 64,
  O_NONBLOCK = 128,
  O_RD_NONBLOCK = 256,
  O_WR_NONBLOCK = 512,
} FileModes ;

int create(const char* file_path, unsigned short file_attr) ;
int open(const char* file_name, uint32_t mode) ;
int openstream(uint32_t mode);
int close(int fd) ;
int unlink(const char* filePath);

int mkdir(const char* dirPath, uint16_t attr);
int get_dir_content(const char* dirPath, struct stat_ex** dirList, int* size);

int read(int fd, void* buf, int len) ;
int write(int fd, const void* buf, int len) ;
void select(io_descriptor* waitIODescriptors, io_descriptor* readyIODescriptors);
int lseek(int fd, int offset, int seekType) ;
unsigned tell(int fd) ;
int getomode(int fd) ;
int access(const char* szFileName, int mode) ;
int dup2(int oldFD, int newFD) ;
int getcwd(char* buf, size_t size);
int chdir(const char* dirPath);

#if defined __cplusplus
}
#endif


#endif
