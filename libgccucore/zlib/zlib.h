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
#include <zbuffer.h>

/* Maximum value for windowBits in deflateInit2 and inflateInit2.
 * WARNING: reducing MAX_WBITS makes minigzip unable to extract .gz files
 * created by gzip. (Files created by minigzip can still be extracted by
 * gzip.)
 */
#define MAX_WBITS 15 /* 32K LZ77 window */
#define DEF_WBITS MAX_WBITS

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
#define Z_BLOCK         5
#define Z_TREES         6

#define Z_OK 0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
//#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
///* Return codes for the compression/decompression functions. Negative values
// * are errors, positive values are used for special but normal events.
// */
//
//#define Z_NO_COMPRESSION         0
//#define Z_BEST_SPEED             1
//#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
///* compression levels */
//
#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_RLE                 3
#define Z_FIXED               4
#define Z_DEFAULT_STRATEGY    0
///* compression strategy; see deflateInit2() below for details */
//
#define Z_BINARY   0
#define Z_TEXT     1
//#define Z_ASCII    Z_TEXT   /* for compatibility with 1.2.2 and earlier */
#define Z_UNKNOWN  2
///* Possible values of the data_type field for deflate() */
//
#define Z_DEFLATED 8

#define DEF_MEM_LEVEL 8
#define MAX_MEM_LEVEL DEF_MEM_LEVEL

#define STATIC_TREES 1
#define DYN_TREES    2

/* Reverse the bytes in a 32-bit value */
#define ZSWAP32(q) ((((q) >> 24) & 0xff) + (((q) >> 8) & 0xff00) + \
                    (((q) & 0xff00) << 8) + (((q) & 0xff) << 24))

#define MIN_MATCH  3
#define MAX_MATCH  258

#define PRESET_DICT 0x20
#define OS_CODE  3
#define STORED_BLOCK 0

namespace gccucore {
  namespace zlib {
    char* decode(const char*);
    int bitreverse16(int n);
    int bit_reverse(int v, int bits);
    unsigned long adler32(unsigned long adler, const byte* buf, uint32_t len);
    int compress2 (byte* dest, unsigned long* destLen, const byte* source, unsigned long sourceLen, int level);
    int compress (byte* dest, unsigned long* destLen, const byte* source, unsigned longsourceLen);
    unsigned long compressBound (unsigned long sourceLen);

    typedef void* (*alloc_func)(void* opaque, uint32_t items, uint32_t size);
    typedef void   (*free_func)(void* opaque, void* address);

    typedef struct z_stream_s {
      const byte*   next_in;     /* next input byte */
      uint32_t      avail_in;  /* number of bytes available at next_in */
      unsigned long total_in;  /* total number of input bytes read so far */

      byte*         next_out; /* next output byte will go here */
      uint32_t      avail_out; /* remaining free space at next_out */
      unsigned long total_out; /* total number of bytes output so far */

      const char*   msg;  /* last error message, NULL if no error */
      struct internal_state* state; /* not visible by applications */

      alloc_func zalloc;  /* used to allocate the internal state */
      free_func  zfree;   /* used to free the internal state */
      void*      opaque;  /* private data object passed to zalloc and zfree */

      int        data_type;  /* best guess about the data type: binary or text
                               for deflate, or the decoding state for inflate */
      unsigned long   adler;      /* Adler-32 or CRC-32 value of the uncompressed data */
      unsigned long   reserved;   /* reserved for future use */
    } z_stream;

    /*
      gzip header information passed to and from zlib routines.  See RFC 1952
      for more details on the meanings of these fields.
    */
    typedef struct gz_header_s {
      int     text;       /* true if compressed data believed to be text */
      unsigned long   time;       /* modification time */
      int     xflags;     /* extra flags (not used when writing a gzip file) */
      int     os;         /* operating system */
      byte   *extra;     /* pointer to extra field or Z_NULL if none */
      uint32_t    extra_len;  /* extra field length (valid if extra != Z_NULL) */
      uint32_t    extra_max;  /* space at extra (only when reading header) */
      byte   *name;      /* pointer to zero-terminated file name or Z_NULL */
      uint32_t    name_max;   /* space at name (only when reading header) */
      byte   *comment;   /* pointer to zero-terminated comment or Z_NULL */
      uint32_t    comm_max;   /* space at comment (only when reading header) */
      int     hcrc;       /* true if there was or will be a header crc */
      int     done;       /* true when done reading gzip header (not used when writing a gzip file) */
    } gz_header;

    extern const char ZLIB_VERSION[10];
    extern const char* const z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]
#define ERR_RETURN(strm,err) return (strm->msg = ERR_MSG(err), (err))
  };
}

