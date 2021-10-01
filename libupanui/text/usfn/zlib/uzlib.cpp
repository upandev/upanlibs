/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2015 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
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

#include <uzlib.h>

namespace upanui {
  namespace usfn {
    int zlib::bitreverse16(int n) {
      n = ((n & 0xAAAA) >>  1) | ((n & 0x5555) << 1);
      n = ((n & 0xCCCC) >>  2) | ((n & 0x3333) << 2);
      n = ((n & 0xF0F0) >>  4) | ((n & 0x0F0F) << 4);
      n = ((n & 0xFF00) >>  8) | ((n & 0x00FF) << 8);
      return n;
    }

    int zlib::bit_reverse(int v, int bits) {
      return bitreverse16(v) >> (16-bits);
    }

    char* zlib::decode(const char *buffer) {
      zbuffer zbuf((unsigned char *) buffer);
      return zbuf.parse();
    }
  }
}