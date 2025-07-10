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

#define INET_ADDRSTRLEN 16

typedef enum {
  IPPROTO_IP = 0,
  IPPROTO_ICMP = 1,
  IPPROTO_TCP = 6,
  IPPROTO_UDP = 17,
} IPPROTO_TYPE;

typedef uint32_t in_addr_t;

struct in_addr {
  in_addr_t s_addr;
};

struct ip {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  unsigned int ip_hl:4;       // header length
  unsigned int ip_v:4;        // version
#else
  unsigned int ip_v:4;
  unsigned int ip_hl:4;
#endif
  uint8_t  ip_tos;            // type of service
  uint16_t ip_len;            // total length
  uint16_t ip_id;             // identification
  uint16_t ip_off;            // fragment offset field
  uint8_t  ip_ttl;            // time to live
  uint8_t  ip_p;              // protocol
  uint16_t ip_sum;            // checksum
  struct in_addr ip_src; // source address
  struct in_addr ip_dst; // dest address
} PACKED;

char* inet_ntoa(struct in_addr in);
int inet_aton(const char* ip, struct in_addr* inp);

#if defined __cplusplus
}
#endif