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

#include <stdlib.h>
#include <sys/socket.h>

#if defined __cplusplus
extern "C" {
#endif

struct hostent {
  char  *h_name;        // Official name of the host
  char **h_aliases;     // Null-terminated array of alternate names
  int    h_addrtype;    // Address type (e.g., AF_INET)
  int    h_length;      // Length of address in bytes
  char **h_addr_list;   // Null-terminated array of addresses
  // (use h_addr_list[0] for the first address)
};

struct servent {
  char  *s_name;      /* Official service name */
  char **s_aliases;   /* Alias list (NULL-terminated array of strings) */
  int    s_port;      /* Port number (in network byte order) */
  char  *s_proto;     /* Protocol name, e.g., "tcp" or "udp" */
};

struct hostent* gethostbyname(const char* name);
struct hostent* gethostbyaddr(const void *addr, socklen_t len, int type);
void freehostinfo(struct hostent* hostinfo);

struct servent* getservbyname(const char *name, const char *proto);
struct servent* getservbyport(int port, const char *proto);
struct servent* getservent();
void setservent(int stayopen);
void endservent();

#if defined __cplusplus
}
#endif