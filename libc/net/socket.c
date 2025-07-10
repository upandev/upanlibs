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

#include <net/socket.h>
#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <syscalldefs.h>

uint16_t htons(uint16_t x) {
#if __BYTE_ORDER == __BIG_ENDIAN
  return x;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
  return __builtin_bswap16(x);
#else
# error "unknown __BYTE_ORDER for this machine"
#endif
}
weak_alias(htons, ntohs)

uint32_t htonl(uint32_t x) {
#if __BYTE_ORDER == __BIG_ENDIAN
  return x;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
  return __builtin_bswap32(x);
#else
# error "unknown __BYTE_ORDER for this machine"
#endif
}
weak_alias(htonl, ntohl)

const uint8_t INADDR_MAC_BROADCAST[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

__thread char _inet_conversion_buffer[INET_ADDRSTRLEN + 1];

char* inet_ntoa(struct in_addr in) {
  const uint8_t* addr = (uint8_t*) &in;
  int n = snprintf(_inet_conversion_buffer, INET_ADDRSTRLEN + 1, "%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]);
  if (n >= INET_ADDRSTRLEN) {
    return NULL;
  }
  return _inet_conversion_buffer;
}

int inet_aton(const char* ip, struct in_addr* inp) {
  if (!ip || !inp) {
    return 0;
  }

  if (strlen(ip) >= INET_ADDRSTRLEN) {
    return 0;
  }

  uint8_t res[4];

  char c[4];
  int ci = 0;
  const char* p = ip;
  int dot_count = 0;

  while(*p != '\0') {
    if (*p == '.') {
      if (dot_count == 3) {
        return 0;
      }
      ++dot_count;

      c[ci] = '\0';
      ci = 0;

      const uint32_t v = atoi(c);
      if (v > 0xFF) {
        return 0;
      }
      res[dot_count - 1] = v;
    } else {
      if (ci == 3) {
        return 0;
      }
      if (!isdigit(*p)) {
        return 0;
      }
      c[ci] = *p;
      ++ci;
    }
    ++p;
  }

  if (dot_count != 3) {
    return 0;
  }

  c[ci] = '\0';
  const uint32_t v = atoi(c);
  if (v > 0xFF) {
    return 0;
  }
  res[dot_count] = v;
  inp->s_addr = *((in_addr_t*)(&res));

  return 1;
}

sock_t socket(SA_FAMILY_TYPE sa_family, SOCKET_TYPE socket_type, int protocol) {
  return SysNet_CreateSocket(sa_family, socket_type, protocol);
}

int bind(sock_t fd, struct sockaddr* client_addr, socklen_t len) {
  if (!client_addr) {
    return -1;
  }
  return SysNet_Bind(fd, client_addr, len);
}

int setsockopt(sock_t fd, int level, SOCKET_OPTION option, const void* optval, socklen_t len) {
  return SysNet_SetSockOpt(fd, level, option, optval, len);
}

ssize_t sendto(int fd, const void *buf, size_t n, int flags, const struct sockaddr* addr, socklen_t len) {
  return SysNet_SendTo(fd, buf, n, flags, addr, len);
}

ssize_t recvfrom(int fd, void *buf, size_t n, int flags, struct sockaddr* addr, socklen_t* len) {
  return SysNet_RecvFrom(fd, buf, n, flags, addr, len);
}