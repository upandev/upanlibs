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

#pragma once

#include <stdlib.h>
#include <huffman.h>

namespace upanui {
  namespace usfn {
    class zbuffer {
    public:
      zbuffer(unsigned char* buffer);
      char* parse();

    private:
      static unsigned char ZDEFAULT_LENGTH[288], ZDEFAULT_DISTANCE[32];

      unsigned char get8();
      void fill_bits();
      uint32_t receive(int n);
      char* expand(char* cur_zout);
      void parse_uncompressed_block();
      void compute_huffman_codes();
      int huffman_decode(huffman& z);
      int huffman_decode_slowpath(huffman& z);
      void parse_huffman_block();

      unsigned char* _buffer;
      int num_bits;
      uint32_t code_buffer;

      char *zout;
      char *zout_start;
      char *zout_end;

      huffman z_length, z_distance;
    };
  }
}