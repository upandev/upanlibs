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

#include <netdb.h>
#include <syscalldefs.h>

//TODO: make it thread local
int h_errno;

struct hostent *gethostbyname(const char *name) {
  struct hostent *hostinfo;
  if (SysNet_GetHostByName(name, &hostinfo) < 0) {
    return NULL;
  }
  return hostinfo;
}

struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type) {
  struct hostent *hostinfo;
  if (SysNet_GetHostByAddr(addr, len, type, &hostinfo) < 0) {
    return NULL;
  }
  return hostinfo;
}

void freehostinfo(struct hostent *hostinfo) {
  SysNet_FreeHostInfo(hostinfo);
}