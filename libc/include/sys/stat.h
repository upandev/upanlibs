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
#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

# include <dtime.h>

#if defined __cplusplus
extern "C" {
#endif

/* User (owner) permissions */
#define S_IRUSR  00400   /* read */
#define S_IWUSR  00200   /* write */
#define S_IXUSR  00100   /* execute */

/* Group permissions */
#define S_IRGRP  00040
#define S_IWGRP  00020
#define S_IXGRP  00010

/* Other (world) permissions */
#define S_IROTH  00004
#define S_IWOTH  00002
#define S_IXOTH  00001

#define S_ISUID	04000
#define S_ISGID	02000
#define S_ISVTX 01000

/* Combined macros */
#define S_IRWXU  (S_IRUSR | S_IWUSR | S_IXUSR)   /* 0700 */
#define S_IRWXG  (S_IRGRP | S_IWGRP | S_IXGRP)   /* 0070 */
#define S_IRWXO  (S_IROTH | S_IWOTH | S_IXOTH)   /* 0007 */

/* This is same as FileSystem_FileStat in Upanix Source and must be maintained in consistent with that */
struct stat {
  int 	    st_dev;     /* ID of device containing file */
  int     	st_ino;     /* inode number */
  uint16_t 	st_mode;    /* protection */
  int   		st_nlink;   /* number of hard links */
  int     	st_uid;     /* user ID of owner */
  int     	st_gid;     /* group ID of owner */
  int     	st_rdev;    /* device ID (if special file) */
  uint32_t  st_size;    /* total size, in bytes */
  uint32_t  st_blksize; /* blocksize for filesystem I/O */
  uint32_t  st_blocks;  /* number of blocks allocated */

  struct timeval st_atime;   /* time of last access */
  struct timeval st_mtime;   /* time of last modification */
  struct timeval st_ctime;   /* time of last status change */
};

int stat(const char* path, struct stat* st);
int lstat(const char* path, struct stat* st);
int fstat(int fd, struct stat* st) ;
int chmod(const char *path, mode_t mode);

#if defined __cplusplus
}
#endif

#endif