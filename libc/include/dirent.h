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
#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

struct dirent {
  ino_t    d_ino;      /* inode number */
  char     d_name[33];   /* filename (null-terminated) */
  struct stat d_stat;
//  off_t    d_off;      /* offset to the next dirent */
//  uint16_t d_reclen;   /* length of this record */
//  uint8_t  d_type;     /* type of file */
};

struct __dirstream {
  int fd;                 /* File descriptor for the directory */
  struct stat d_stat;
  size_t size;             /* number of entries in the block */
  size_t index;           /* Current offset into the block */
  struct dirent *data;     /* Directory block */
};

typedef struct __dirstream DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#if defined __cplusplus
}
#endif
