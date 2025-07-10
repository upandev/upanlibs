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

#define ETH_ALEN 6

typedef enum {
  ETH_P_IP = 0x0800,
  ETH_P_ARP = 0x0806,
} ETH_PROTO_TYPE;

struct ethhdr {
  uint8_t h_dest[ETH_ALEN];
  uint8_t h_source[ETH_ALEN];
  uint16_t h_proto;
} PACKED;

#if defined __cplusplus
}
#endif