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

#include <zlib.h>

namespace gccucore {
  namespace zlib {
    const char ZLIB_VERSION[10] = "1.2.11";

    const char * const z_errmsg[10] = {
      (const char *)"need dictionary",     /* Z_NEED_DICT       2  */
      (const char *)"stream end",          /* Z_STREAM_END      1  */
      (const char *)"",                    /* Z_OK              0  */
      (const char *)"file error",          /* Z_ERRNO         (-1) */
      (const char *)"stream error",        /* Z_STREAM_ERROR  (-2) */
      (const char *)"data error",          /* Z_DATA_ERROR    (-3) */
      (const char *)"insufficient memory", /* Z_MEM_ERROR     (-4) */
      (const char *)"buffer error",        /* Z_BUF_ERROR     (-5) */
      (const char *)"incompatible version",/* Z_VERSION_ERROR (-6) */
      (const char *)""
    };

    int bitreverse16(int n) {
      n = ((n & 0xAAAA) >>  1) | ((n & 0x5555) << 1);
      n = ((n & 0xCCCC) >>  2) | ((n & 0x3333) << 2);
      n = ((n & 0xF0F0) >>  4) | ((n & 0x0F0F) << 4);
      n = ((n & 0xFF00) >>  8) | ((n & 0x00FF) << 8);
      return n;
    }

    int bit_reverse(int v, int bits) {
      return bitreverse16(v) >> (16-bits);
    }

    char* decode(const char *buffer) {
      zbuffer zbuf((unsigned char *) buffer);
      return zbuf.parse();
    }
  }
}
