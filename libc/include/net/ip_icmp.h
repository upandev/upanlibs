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
#include <net/ip.h>

#if defined __cplusplus
extern "C" {
#endif

#define ICMP_ECHOREPLY 0
#define ICMP_ECHO 8

struct icmp {
  uint8_t  icmp_type;
  uint8_t  icmp_code; //subtype
  uint16_t icmp_cksum;
  union {
    struct {
      uint16_t icmp_id;
      uint16_t icmp_seq;
    };             // for echo request/reply
    uint32_t icmp_gateway;
    struct {
      uint16_t __unused;
      uint16_t mtu;
    };
  };//header-union

  union {
    uint32_t icmp_timestamp[3];
    uint8_t icmp_data[1];
    struct ip ip_header; // for errors (includes offending IP header)
    uint32_t unused;
  };//data-union
} PACKED;

#if defined __cplusplus
}
#endif