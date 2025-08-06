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

#ifndef _NETINET_IN_H_
#define _NETINET_IN_H_

#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  AF_LOCAL = 1,
  AF_UNSPEC = 0,
  AF_INET = 2,
  AF_INET6 = 10,
  AF_PACKET = 17,
} SA_FAMILY_TYPE;

typedef uint32_t in_addr_t;

struct in_addr {
  in_addr_t s_addr;
};

struct in6_addr {
  uint8_t s6_addr[16];
};

typedef uint16_t sa_family_t;
typedef uint16_t in_port_t;

struct sockaddr_in {
  sa_family_t sin_family;
  in_port_t sin_port;
  struct in_addr sin_addr;
  uint8_t sin_zero[8];
} PACKED;

struct sockaddr_in6 {
  sa_family_t     sin6_family;   /* AF_INET6 */
  in_port_t       sin6_port;     /* Port number (network byte order) */
  uint32_t        sin6_flowinfo; /* IPv6 traffic class & flow info */
  struct in6_addr sin6_addr;     /* IPv6 address */
  uint32_t        sin6_scope_id; /* Scope ID (e.g., interface index) */
} PACKED;

#define _SS_SIZE     128
#define _SS_ALIGN    sizeof(int64_t)
#define _SS_PAD1SIZE (_SS_ALIGN - sizeof(sa_family_t))
#define _SS_PAD2SIZE (_SS_SIZE - (sizeof(sa_family_t) + _SS_PAD1SIZE + _SS_ALIGN))

struct sockaddr_storage {
  sa_family_t ss_family;      /* Address family */
  char __ss_pad1[_SS_PAD1SIZE];
  int64_t __ss_align;         /* Force desired alignment */
  char __ss_pad2[_SS_PAD2SIZE];
};

uint16_t htons(uint16_t x);
uint16_t ntohs(uint16_t x);

uint32_t htonl(uint32_t x);
uint32_t ntohl(uint32_t x);

#if defined __cplusplus
}
#endif

#endif