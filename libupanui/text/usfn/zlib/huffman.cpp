/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2015 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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

#include <huffman.h>
#include <string.h>
#include <exception.h>
#include <uzlib.h>

namespace upanui {
  namespace usfn {
    void huffman::build(unsigned char *sizelist, int num) {
      int k=0;
      int code, next_code[16], sizes[17];
      memset(sizes, 0, sizeof(sizes));
      memset(fast, 0, sizeof(fast));

      for (int i=0; i < num; ++i) {
        ++sizes[sizelist[i]];
      }
      sizes[0] = 0;

      for (int i=1; i < 16; ++i) {
        if (sizes[i] > (1 << i))
          throw upan::exception(XLOC, "invalid compressed file - failed to init huffman");
      }
      code = 0;

      for (int i=1; i < 16; ++i) {
        next_code[i] = code;
        firstcode[i] = (uint16_t) code;
        firstsymbol[i] = (uint16_t) k;
        code = (code + sizes[i]);
        if (sizes[i])
          if (code-1 >= (1 << i)) {
            throw upan::exception(XLOC, "invalid compressed file - failed to init huffman");
          }
        maxcode[i] = code << (16-i);
          code <<= 1;
          k += sizes[i];
      }
      maxcode[16] = 0x10000;

      for (int i = 0; i < num; ++i) {
        int s = sizelist[i];
        if (s) {
          int c = next_code[s] - firstcode[s] + firstsymbol[s];
          uint16_t fastv = (uint16_t) ((s << 9) | i);
          size [c] = (unsigned char) s;
          value[c] = (uint16_t) i;
          if (s <= ZFAST_BITS) {
            int j = zlib::bit_reverse(next_code[s], s);
            while (j < (1 << ZFAST_BITS)) {
              fast[j] = fastv;
              j += (1 << s);
            }
          }
          ++next_code[s];
        }
      }
    }
  }
}