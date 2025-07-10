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
#include <net/socket.h>

#if defined __cplusplus
extern "C" {
#endif

#define IFNAMSIZ 16

struct ifmap {
  unsigned long mem_start;   // Start of memory mapped region
  unsigned long mem_end;     // End of memory mapped region
  unsigned short base_addr;  // I/O port base address
  unsigned char irq;         // Interrupt line
  unsigned char dma;         // DMA channel
  unsigned char port;        // Port number
};
  
struct ifreq {
  char ifr_name[IFNAMSIZ];  // Interface name, e.g., "eth0"

  union {
    struct sockaddr ifr_addr;     // IP address
    struct sockaddr ifr_dstaddr;  // P2P destination address
    struct sockaddr ifr_broadaddr;// Broadcast address
    struct sockaddr ifr_netmask;  // Netmask
    struct sockaddr ifr_hwaddr;   // MAC address
    short           ifr_flags;    // Interface flags (e.g., IFF_UP)
    int             ifr_ifindex;  // Interface index
    int             ifr_metric;   // Metric
    int             ifr_mtu;      // MTU size
    struct ifmap    ifr_map;      // Device-specific mapping
    char            ifr_slave[IFNAMSIZ];  // Slave device name
    char            ifr_newname[IFNAMSIZ];// Interface rename
    void *          ifr_data;     // Pointer to arbitrary data
  };
};

#if defined __cplusplus
}
#endif