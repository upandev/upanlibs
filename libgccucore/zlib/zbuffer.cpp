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

#include <zbuffer.h>
#include <exception.h>
#include <zlib.h>

namespace gccucore {
  unsigned char zbuffer::ZDEFAULT_LENGTH[288], zbuffer::ZDEFAULT_DISTANCE[32];

  zbuffer::zbuffer(unsigned char *buffer) : _buffer(buffer), num_bits(0), code_buffer(0) {
    char *p = (char*)malloc(8);
    if (p == NULL) {
      throw upan::exception(XLOC, "zbuffer - mem allocation failed");
    }
    zout_start = p;
    zout = p;
    zout_end = p + 8;

    int i;
    for (i = 0; i <= 143; ++i) ZDEFAULT_LENGTH[i] = 8;
    for (; i <= 255; ++i) ZDEFAULT_LENGTH[i] = 9;
    for (; i <= 279; ++i) ZDEFAULT_LENGTH[i] = 7;
    for (; i <= 287; ++i) ZDEFAULT_LENGTH[i] = 8;

    for (i = 0; i <= 31; ++i) ZDEFAULT_DISTANCE[i] = 5;
  }

  unsigned char zbuffer::get8(){
    return *_buffer++;
  }

  void zbuffer::fill_bits() {
    do {
      code_buffer |= (unsigned int)get8() << num_bits;
      num_bits += 8;
    } while (num_bits <= 24);
  }

  uint32_t zbuffer::receive(int n) {
    if (num_bits < n)
      fill_bits();
    uint32_t k = code_buffer & ((1 << n) - 1);
    code_buffer >>= n;
    num_bits -= n;
    return k;
  }

  char* zbuffer:: expand(char* cur_zout) {
    zout = cur_zout;
    int cur = (int) (zout - zout_start);
    int limit = (int) (zout_end - zout_start);
    if(limit == 8) {
      if(zout_start[0] != 'S' || zout_start[1] != 'F' || zout_start[2] != 'N')  {
        throw upan::exception(XLOC, "unzip failed - compressed data is not SFN");
      }
      limit = *((uint32_t*)&zout_start[4]);
    } else {
      throw upan::exception(XLOC, "unzip failed - limit != 8");
    }
    char* q = (char *)realloc(zout_start, limit);
    if (q == NULL) {
      throw upan::exception(XLOC, "unzip failed - realloc failed");
    }
    zout_start = q;
    zout       = q + cur;
    zout_end   = q + limit;
    return zout;
  }

  void zbuffer::parse_uncompressed_block() {
    if (num_bits & 7)
      receive(num_bits & 7);

    int k = 0;
    unsigned char header[4];
    while (num_bits > 0) {
      header[k++] = (unsigned char) (code_buffer & 255);
      code_buffer >>= 8;
      num_bits -= 8;
    }
    while (k < 4)
      header[k++] = get8();
    int len  = header[1] * 256 + header[0];
    int nlen = header[3] * 256 + header[2];
    if (nlen != (len ^ 0xffff)) {
      throw upan::exception(XLOC, "parse_uncompressed_block failed");
    }
    if (zout + len > zout_end) {
      expand(zout);
    }
    for(k = 0; k < len; k++)
      zout[k] = _buffer[k];
    _buffer += len;
    zout += len;
  }

  int zbuffer::huffman_decode_slowpath(huffman& z) {
    int k = zlib::bit_reverse(code_buffer, 16);
    int s;
    for (s = huffman::ZFAST_BITS + 1; ; ++s)
      if (k < z.getMaxCodeAt(s))
        break;
    if (s == 16) return -1;
    int b = (k >> (16-s)) - z.getFirstCodeAt(s) + z.getFirstSymbolAt(s);
    code_buffer >>= s;
    num_bits -= s;
    return z.getValueAt(b);
  }

  int zbuffer::huffman_decode(huffman& z) {
    if (num_bits < 16) {
      fill_bits();
    }

    int b = z.getFastAt(code_buffer & huffman::ZFAST_MASK);
    if (b) {
      int s = b >> 9;
      code_buffer >>= s;
      num_bits -= s;
      return b & 511;
    }

    return huffman_decode_slowpath(z);
  }

  void zbuffer::compute_huffman_codes()
  {
    static unsigned char length_dezigzag[19] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
    huffman z_codelength;
    unsigned char lencodes[286+32+137];
    unsigned char codelength_sizes[19];
    int i,n;

    int hlit  = receive(5) + 257;
    int hdist = receive(5) + 1;
    int hclen = receive(4) + 4;
    int ntot  = hlit + hdist;

    memset(codelength_sizes, 0, sizeof(codelength_sizes));
    for (i=0; i < hclen; ++i) {
      int s = receive(3);
      codelength_sizes[length_dezigzag[i]] = (unsigned char) s;
    }

    z_codelength.build(codelength_sizes, 19);

    n = 0;
    while (n < ntot) {
      int c = huffman_decode(z_codelength);
      if (c < 0 || c >= 19) {
        throw upan::exception(XLOC, "huffman decode error, c = %d", c);
      }
      if (c < 16)
        lencodes[n++] = (unsigned char) c;
      else {
        unsigned char fill = 0;
        if (c == 16) {
          c = receive(2) + 3;
          if (n == 0) {
            throw upan::exception(XLOC, "huffman decode error, n = %d", n);
          }
          fill = lencodes[n-1];
        } else if (c == 17)
          c = receive(3) + 3;
        else {
          c = receive(7) + 11;
        }
        if (ntot - n < c) {
          throw upan::exception(XLOC, "huffman decode error, ntot -n < c");
        }
        memset(lencodes+n, fill, c);
        n += c;
      }
    }
    if (n != ntot) {
      throw upan::exception(XLOC, "huffman decode error, n != ntot, n = %d, ntot = %d", n, ntot);
    }
    z_length.build(lencodes, hlit);
    z_distance.build(lencodes + hlit, hdist);
  }

  static int __zlength_base[31] = {
      3,4,5,6,7,8,9,10,11,13,
      15,17,19,23,27,31,35,43,51,59,
      67,83,99,115,131,163,195,227,258,0,0 };

  static int __zlength_extra[31]=
      { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };

  static int __zdist_base[32] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
                                  257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};

  static int __zdist_extra[32] =
      { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

  void zbuffer::parse_huffman_block() {
    char* cur_zout = zout;
    for(;;) {
      int z = huffman_decode(z_length);
      if (z < 256) {
        if (z < 0) {
          throw upan::exception(XLOC, "failed to parse huffman block @ 1");
        }
        if (cur_zout >= zout_end) {
          cur_zout = expand(cur_zout);
        }
        *cur_zout++ = (char) z;
      } else {
        unsigned char *p;
        int len,dist;
        if (z == 256) {
          zout = cur_zout;
          return;
        }
        z -= 257;
        len = __zlength_base[z];
        if (__zlength_extra[z]) len += receive(__zlength_extra[z]);
        z = huffman_decode(z_distance);
        if (z < 0) {
          throw upan::exception(XLOC, "failed to parse huffman block @ 2");
        }
        dist = __zdist_base[z];
        if (__zdist_extra[z]) dist += receive(__zdist_extra[z]);
        if (cur_zout - zout_start < dist) {
          throw upan::exception(XLOC, "failed to parse huffman block @ 3");
        }
        if (cur_zout + len > zout_end) {
          cur_zout = expand(cur_zout);
        }
        p = (unsigned char *) (cur_zout - dist);
        if (dist == 1) {
          unsigned char v = *p;
          if (len) { do *cur_zout++ = v; while (--len); }
        } else {
          if (len) { do *cur_zout++ = *p++; while (--len); }
        }
      }
    }
  }

  char* zbuffer::parse() {
    try {
      int fin;
      do {
        fin = receive(1);
        int type = receive(2);
        if (type == 0) {
          parse_uncompressed_block();
        } else if (type == 3) {
          throw upan::exception(XLOC, "can't uncompress - type = 3");
        } else {
          if (type == 1) {
            z_length.build(ZDEFAULT_LENGTH, 288);
            z_distance.build(ZDEFAULT_DISTANCE, 32);
          } else {
            compute_huffman_codes();
          }
          parse_huffman_block();
        }
      } while (!fin);
      return zout_start;
    } catch(upan::exception& e) {
      free(zout_start);
      throw e;
    }
  }
}
