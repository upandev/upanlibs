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
#include <net/if_ether.h>

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  SOCK_STREAM = 1,
  SOCK_DGRAM = 2,
  SOCK_RAW = 3
} SOCKET_TYPE;

typedef enum {
  AF_INET = 2,
  AF_PACKET = 17,
} SA_FAMILY_TYPE;

typedef enum {
  SO_BROADCAST,
  SO_RCVTIMEO,
  IP_TTL,
  TCP_NODELAY
} SOCKET_OPTION;

/* Bits in the FLAGS argument to `send', `recv', et al.  */
typedef enum {
  MSG_OOB = 0x01,	/* Process out-of-band data.  */
  MSG_PEEK = 0x02,	/* Peek at incoming messages.  */
  MSG_DONTROUTE = 0x04,	/* Don't use local routing.  */
  MSG_CTRUNC = 0x08,	/* Control data lost before delivery.  */
  MSG_PROXY = 0x10,	/* Supply or ask second address.  */
  MSG_TRUNC = 0x20,
  MSG_DONTWAIT = 0x40, /* Nonblocking IO.  */
  MSG_EOR = 0x80, /* End of record.  */
  MSG_WAITALL = 0x100, /* Wait for a full request.  */
  MSG_FIN = 0x200,
  MSG_SYN = 0x400,
  MSG_CONFIRM = 0x800, /* Confirm path validity.  */
  MSG_RST = 0x1000,
  MSG_ERRQUEUE = 0x2000, /* Fetch message from error queue.  */
  MSG_NOSIGNAL = 0x4000, /* Do not generate SIGPIPE.  */
  MSG_MORE = 0x8000,  /* Sender will send more.  */
  MSG_WAITFORONE = 0x10000, /* Wait for at least one packet to return.*/
  MSG_BATCH = 0x40000, /* sendmmsg: more messages coming.  */
  MSG_ZEROCOPY = 0x4000000, /* Use user data in kernel path.  */
  MSG_FASTOPEN = 0x20000000, /* Send data in TCP SYN.  */
  MSG_CMSG_CLOEXEC = 0x40000000	/* Set close_on_exit for file descriptor received through  SCM_RIGHTS */
} SOCKET_SEND_RECV_FLAGS;

typedef uint16_t sa_family_t;
typedef uint16_t in_port_t;
typedef uint32_t socklen_t;
typedef int sock_t;

#define INADDR_ANY (in_addr_t)0
#define INADDR_LOOPBACK (in_addr_t)0x0100007f
#define INADDR_BROADCAST (in_addr_t)0xffffffff
#define INADDR_NONE (in_addr_t)0xffffffff

#define INPORT_ANY (in_port_t)0

#define SOL_SOCKET 1

struct sockaddr {
  sa_family_t sa_family;
  uint8_t sa_data[14];
} PACKED;

struct sockaddr_in {
  sa_family_t sin_family;
  in_port_t sin_port;
  struct in_addr sin_addr;
  uint8_t sin_zero[8];
} PACKED;

struct sockaddr_ll {
  uint16_t sll_family;   // Always AF_PACKET
  uint16_t sll_protocol; // Ethernet protocol in network byte order (e.g., htons(ETH_P_ARP))
  int      sll_ifindex;  // Interface index (e.g., from ioctl SIOCGIFINDEX)
  uint16_t sll_hatype;   // ARP hardware type (e.g., ARPHRD_ETHER)
  uint8_t  sll_pkttype;  // Packet type (e.g., PACKET_HOST)
  uint8_t  sll_halen;    // Length of address (MAC length = 6)
  uint8_t  sll_addr[8];  // Physical address (MAC address, 6 bytes + padding)
} PACKED;

extern const uint8_t INADDR_MAC_BROADCAST[ETH_ALEN];

uint16_t htons(uint16_t x);
uint16_t ntohs(uint16_t x);

uint32_t htonl(uint32_t x);
uint32_t ntohl(uint32_t x);

sock_t socket(SA_FAMILY_TYPE sa_family, SOCKET_TYPE socket_type, int protocol);
int bind(sock_t fd, struct sockaddr* client_addr, socklen_t len);
int setsockopt(sock_t fd, int level, SOCKET_OPTION option, const void* optval, socklen_t len);
ssize_t sendto(int fd, const void *buf, size_t n, int flags, const struct sockaddr* addr, socklen_t len);
ssize_t recvfrom(int fd, void *buf, size_t n, int flags, struct sockaddr* addr, socklen_t* len);

#if defined __cplusplus
}
#endif