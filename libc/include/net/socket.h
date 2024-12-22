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

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  SOCK_STREAM = 1,
  SOCK_DGRAM = 2
} SOCKET_TYPE;

typedef enum {
  AF_INET = 2
} SA_FAMILY_TYPE;

typedef enum {
  IPPROTO_IP = 0,
  IPPROTO_TCP = 6,
  IPPROTO_UDP = 17,
} IPPROTO_TYPE;

typedef uint16_t sa_family_t;
typedef uint16_t in_port_t;
typedef uint32_t socklen_t;
typedef uint32_t in_addr_t;
typedef int sock_t;

#define INADDR_ANY (in_addr_t)0
#define INADDR_LOOPBACK (in_addr_t)0x7f000001
#define INADDR_BROADCAST (in_addr_t)0xffffffff
#define INADDR_NONE (in_addr_t)0xffffffff

#define INPORT_ANY (in_port_t)0

#define INET_ADDRSTRLEN 16

struct in_addr {
  in_addr_t s_addr;
};

struct sockaddr {
  sa_family_t sa_family;
  uint8_t sa_data[14];
};

struct sockaddr_in {
  sa_family_t sin_family;
  in_port_t sin_port;
  struct in_addr sin_addr;
  uint8_t sin_zero[8];
};

uint16_t htons(uint16_t x);
uint16_t ntohs(uint16_t x);

uint32_t htonl(uint32_t x);
uint32_t ntohl(uint32_t x);

char* inet_ntoa(struct in_addr in);
in_addr_t inet_aton(const char* ip);

sock_t socket(SA_FAMILY_TYPE sa_family, SOCKET_TYPE socket_type, IPPROTO_TYPE protocol);
int bind(sock_t fd, struct sockaddr* client_addr, socklen_t len);

#if defined __cplusplus
}
#endif