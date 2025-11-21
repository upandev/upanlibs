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

int stat(const char* szFileName, struct stat* pFileStat) ;
int fstat(int iFD, struct stat* pFileStat) ;

#if defined __cplusplus
}
#endif

#endif