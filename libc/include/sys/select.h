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
#ifndef _SELECT_H_
#define _SELECT_H_

#include <stdint.h>
#include <string.h>
#include <time.h>

#if defined __cplusplus
extern "C" {
#endif

typedef uint64_t __fd_mask;

/* Number of descriptors that can fit in an `fd_set'.  */
#define __FD_SETSIZE 1024
#define __NFDBITS (8 * sizeof(__fd_mask))
#define FD_SETSIZE __FD_SETSIZE

typedef struct {
  __fd_mask fds_bits[__FD_SETSIZE / __NFDBITS];
} fd_set;

#define FD_SET(fd, fdsetp) \
    ((fdsetp)->fds_bits[(fd) / __NFDBITS] |= (1UL << ((fd) % __NFDBITS)))

#define FD_CLR(fd, fdsetp) \
    ((fdsetp)->fds_bits[(fd) / __NFDBITS] &= ~(1UL << ((fd) % __NFDBITS)))

#define FD_ISSET(fd, fdsetp) \
    ((fdsetp)->fds_bits[(fd) / __NFDBITS] & (1UL << ((fd) % __NFDBITS)))

#define FD_ZERO(fdsetp) \
    (memset(fdsetp, 0, sizeof(*(fdsetp))))

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

#if defined __cplusplus
}
#endif
#endif