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

#include <netinet/ip.h>
#include <stdio.h>
#include <string.h>

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

bool is_ip_address(const char* ip) {
  struct in_addr addr;
  return inet_aton(ip, &addr) == 1;
}