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
#include <net/if_ether.h>
#include <net/ip.h>

#if defined __cplusplus
extern "C" {
#endif

#define ARPHRD_ETHER 1

#define ARPOP_REQUEST 1
#define ARPOP_REPLY 2

struct arphdr {
  uint16_t ar_hrd;      // Hardware type (e.g., ARPHRD_ETHER)
  uint16_t ar_pro;      // Protocol type (e.g., ETH_P_IP)
  uint8_t ar_hln;     // Hardware address length (6)
  uint8_t ar_pln;     // Protocol address length (4)
  uint16_t ar_op;       // Operation (1=request, 2=reply)
};

struct ether_arp {
  struct arphdr ea_hdr;        // Generic ARP header

  uint8_t arp_sha[ETH_ALEN];   // Sender hardware address (MAC)
  in_addr_t arp_spa;          // Sender protocol address (IPv4)
  uint8_t arp_tha[ETH_ALEN];   // Target hardware address (MAC)
  in_addr_t arp_tpa;          // Target protocol address (IPv4)
} PACKED;

#if defined __cplusplus
}
#endif