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
#include <PngParser.h>

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include <crc32.h>
#include <zlib.h>
#include <inflate.h>
#include <deflate.h>
#include <exception.h>

/* Not build options, edit at your own risk! */
#define PNG_READ_SIZE (8192)
#define PNG_WRITE_SIZE PNG_READ_SIZE
#define PNG_MAX_CHUNK_COUNT (1000)

#define PNG_TARGET_CLONES(x)

#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
  #define PNG_X86

  #if defined(__x86_64__) || defined(_M_X64)
      #undef PNG_TARGET_CLONES
      #define PNG_TARGET_CLONES(x) __attribute__((target_clones(x)))
  #endif
#endif

namespace upanui {
  static void defilter_sub3(size_t rowbytes, unsigned char *row);

  static void defilter_sub4(size_t rowbytes, unsigned char *row);

  static void defilter_avg3(size_t rowbytes, unsigned char *row, const unsigned char *prev);

  static void defilter_avg4(size_t rowbytes, unsigned char *row, const unsigned char *prev);

  static void defilter_paeth3(size_t rowbytes, unsigned char *row, const unsigned char *prev);

  static void defilter_paeth4(size_t rowbytes, unsigned char *row, const unsigned char *prev);

#if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || defined(__BIG_ENDIAN__)
#define PNG_BIG_ENDIAN
#else
#define PNG_LITTLE_ENDIAN
#endif

#define PNG_GET_CHUNK_BOILERPLATE(chunk) \
    if(!_stored.chunk) throw upan::exception(XLOC, _strerror(PNG_ECHUNKAVAIL));

#define PNG_SET_CHUNK_BOILERPLATE(chunk) \
    if(_data == nullptr && !_encode_only) return PNG_ENOSRC;

  static const uint32_t _u32max = INT32_MAX;

  static const uint32_t adam7_x_start[7] = {0, 4, 0, 2, 0, 1, 0};
  static const uint32_t adam7_y_start[7] = {0, 0, 4, 0, 2, 0, 1};
  static const uint32_t adam7_x_delta[7] = {8, 8, 4, 4, 2, 2, 1};
  static const uint32_t adam7_y_delta[7] = {8, 8, 8, 4, 4, 2, 2};

  static const uint8_t png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};

  static const uint8_t type_ihdr[4] = {73, 72, 68, 82};
  static const uint8_t type_plte[4] = {80, 76, 84, 69};
  static const uint8_t type_idat[4] = {73, 68, 65, 84};
  static const uint8_t type_iend[4] = {73, 69, 78, 68};

  static const uint8_t type_trns[4] = {116, 82, 78, 83};
  static const uint8_t type_chrm[4] = {99, 72, 82, 77};
  static const uint8_t type_gama[4] = {103, 65, 77, 65};
  static const uint8_t type_iccp[4] = {105, 67, 67, 80};
  static const uint8_t type_sbit[4] = {115, 66, 73, 84};
  static const uint8_t type_srgb[4] = {115, 82, 71, 66};
  static const uint8_t type_text[4] = {116, 69, 88, 116};
  static const uint8_t type_ztxt[4] = {122, 84, 88, 116};
  static const uint8_t type_itxt[4] = {105, 84, 88, 116};
  static const uint8_t type_bkgd[4] = {98, 75, 71, 68};
  static const uint8_t type_hist[4] = {104, 73, 83, 84};
  static const uint8_t type_phys[4] = {112, 72, 89, 115};
  static const uint8_t type_splt[4] = {115, 80, 76, 84};
  static const uint8_t type_time[4] = {116, 73, 77, 69};

  static const uint8_t type_offs[4] = {111, 70, 70, 115};
  static const uint8_t type_exif[4] = {101, 88, 73, 102};

  static void *_zalloc(void *opaque, uint32_t items, uint32_t size) {
    if (size > SIZE_MAX / items) return nullptr;
    size_t len = (size_t) items * size;
    return malloc(len);
  }

  static void _zfree(void *opqaue, void *ptr) {
    free(ptr);
  }

  static inline uint16_t read_u16(const void *src) {
    auto data = (const byte *) src;
    return (data[0] & 0xFFU) << 8 | (data[1] & 0xFFU);
  }

  static inline uint32_t read_u32(const void *src) {
    auto data = (const byte *) src;
    return (data[0] & 0xFFUL) << 24 | (data[1] & 0xFFUL) << 16 | (data[2] & 0xFFUL) << 8 | (data[3] & 0xFFUL);
  }

  static inline int32_t read_s32(const void *src) {
    return (int32_t) read_u32(src);
  }

  static inline void write_u16(void *dest, uint16_t x) {
    auto data = (byte *) dest;
    data[0] = x >> 8;
    data[1] = x & 0xFF;
  }

  static inline void write_u32(void *dest, uint32_t x) {
    auto data = (byte *) dest;
    data[0] = (x >> 24);
    data[1] = (x >> 16) & 0xFF;
    data[2] = (x >> 8) & 0xFF;
    data[3] = x & 0xFF;
  }

  static inline void write_s32(void *dest, int32_t x) {
    uint32_t n = x;
    write_u32(dest, n);
  }

/* Returns an iterator for 1,2,4,8-bit samples */
  PngParser::_iter::_iter(unsigned bit_depth, const unsigned char *samples) :
    mask((uint8_t) ((1 << bit_depth) - 1)),
    shift_amount(8 - bit_depth),
    initial_shift(8 - bit_depth), bit_depth(bit_depth), samples(samples) {
  }

/* Returns the current sample unpacked, iterates to the next one */
  uint8_t PngParser::_iter::get_sample() {
    uint8_t x = (samples[0] >> shift_amount) & mask;
    shift_amount -= bit_depth;

    if (shift_amount > 7) {
      shift_amount = initial_shift;
      samples++;
    }

    return x;
  }

  static void u16_row_to_host(void *row, size_t size) {
    auto px = (uint16_t *) row;
    size_t i, n = size / 2;

    for (i = 0; i < n; i++) {
      px[i] = read_u16(&px[i]);
    }
  }

  static void u16_row_to_bigendian(void *row, size_t size) {
    uint16_t *px = (uint16_t *) row;
    size_t i, n = size / 2;

    for (i = 0; i < n; i++) {
      write_u16(&px[i], px[i]);
    }
  }

  static void rgb8_row_to_rgba8(const unsigned char *row, byte *out, uint32_t n) {
    uint32_t i;
    for (i = 0; i < n; i++) {
      memcpy(out + i * 4, row + i * 3, 3);
      out[i * 4 + 3] = 255;
    }
  }

  unsigned PngParser::iHeader::num_channels() const {
    switch (color_type) {
      case PNG_COLOR_TYPE_TRUECOLOR:
        return 3;
      case PNG_COLOR_TYPE_GRAYSCALE_ALPHA:
        return 2;
      case PNG_COLOR_TYPE_TRUECOLOR_ALPHA:
        return 4;
      case PNG_COLOR_TYPE_GRAYSCALE:
      case PNG_COLOR_TYPE_INDEXED:
        return 1;
      default:
        return 0;
    }
  }

  static int buffer_read_fn(PngParser *ctx, void *user, void *data, size_t n) {
    return ctx->buffer_read(n);
  }

  PngParser::PngParser(const void *buf, size_t size) :
      _image_options(Z_DEFAULT_COMPRESSION, 15, 8, Z_FILTERED, 0),
      _text_options(Z_DEFAULT_COMPRESSION, 15, 8, Z_DEFAULT_STRATEGY, 1) {
    if (buf == nullptr) {
      throw upan::exception(XLOC, "PNG image buffer input can't be null");
    }

    memset(this, 0, sizeof(PngParser));

    _max_width = _u32max;
    _max_height = _u32max;

    _max_chunk_size = _u32max;
    _chunk_cache_limit = SIZE_MAX;
    _chunk_count_limit = PNG_MAX_CHUNK_COUNT;

    _state = PNG_STATE_INIT;

    _crc_action_critical = PNG_CRC_ERROR;
    _crc_action_ancillary = PNG_CRC_DISCARD;

    _optimize_option = ~0;
    _encode_flags.filter_choice = PNG_FILTER_CHOICE_ALL;

    _encode_only = false;

    _data = (const byte *) buf;
    _bytes_left = size;

    _read_fn = buffer_read_fn;

    _state = PNG_STATE_INPUT;

    read_chunks();
  }

  PngParser::~PngParser() {
    if (_streaming && _stream_buf != nullptr) free(_stream_buf);

    if (!_user.exif) free(_exif.data);

    if (!_user.iccp) free(_iccp.profile);

    uint32_t i;

    if (_splt_list != nullptr && !_user.splt) {
      for (i = 0; i < _n_splt; i++) {
        free(_splt_list[i].entries);
      }
      free(_splt_list);
    }

    if (_text_list != nullptr) {
      for (i = 0; i < _n_text; i++) {
        if (_user.text) break;

        free(_text_list[i].keyword);
        if (_text_list[i].compression_flag) free(_text_list[i].text);
      }
      free(_text_list);
    }

    if (_chunk_list != nullptr && !_user.unknown) {
      for (i = 0; i < _n_chunks; i++) {
        free(_chunk_list[i].data);
      }
      free(_chunk_list);
    }

    if (_deflate) gccucore::zlib::deflateEnd(&_zstream);
    else inflateEnd(&_zstream);

    if (!_user_owns_out_png) free(_out_png);

    free(_gamma_lut16);

    free(_row_buf);
    free(_scanline_buf);
    free(_prev_scanline_buf);
    free(_filtered_scanline_buf);
  }

  int PngParser::buffer_read(size_t n) {
    if (n > _bytes_left) return PNG_IO_EOF;
    _data = _data + _last_read_size;

    _last_read_size = n;
    _bytes_left -= n;

    return 0;
  }

  /* Calculate scanline width in bits, round up to the nearest byte */
  int PngParser::iHeader::calculate_scanline_width(uint32_t width, size_t& scanline_width) const {
    if (!width) return PngParser::PNG_EINTERNAL;

    size_t res = num_channels() * bit_depth;

    if (res > SIZE_MAX / width) return PNG_EOVERFLOW;
    res = res * width;

    res += 15; /* Filter byte + 7 for rounding */

    if (res < 15) return PNG_EOVERFLOW;

    res /= 8;

    if (res > UINT32_MAX) return PNG_EOVERFLOW;

    scanline_width = res;

    return 0;
  }

  int PngParser::calculate_subimages() {
    iHeader *ihdr = &_ihdr;
    struct SubImage *sub = _subimage;

    if (ihdr->interlace_method == 1) {
      sub[0].width = (ihdr->width + 7) >> 3;
      sub[0].height = (ihdr->height + 7) >> 3;
      sub[1].width = (ihdr->width + 3) >> 3;
      sub[1].height = (ihdr->height + 7) >> 3;
      sub[2].width = (ihdr->width + 3) >> 2;
      sub[2].height = (ihdr->height + 3) >> 3;
      sub[3].width = (ihdr->width + 1) >> 2;
      sub[3].height = (ihdr->height + 3) >> 2;
      sub[4].width = (ihdr->width + 1) >> 1;
      sub[4].height = (ihdr->height + 1) >> 2;
      sub[5].width = ihdr->width >> 1;
      sub[5].height = (ihdr->height + 1) >> 1;
      sub[6].width = ihdr->width;
      sub[6].height = ihdr->height >> 1;
    } else {
      sub[0].width = ihdr->width;
      sub[0].height = ihdr->height;
    }

    int i;
    for (i = 0; i < 7; i++) {
      if (sub[i].width == 0 || sub[i].height == 0) continue;

      int ret = ihdr->calculate_scanline_width(sub[i].width, sub[i].scanline_width);
      if (ret) return ret;

      if (sub[_widest_pass].scanline_width < sub[i].scanline_width) _widest_pass = i;

      _last_pass = i;
    }

    return 0;
  }

  int PngParser::check_decode_fmt(const int fmt) const {
    switch (fmt) {
      case PNG_FMT_RGBA8:
      case PNG_FMT_RGBA16:
      case PNG_FMT_RGB8:
      case PNG_FMT_PNG:
      case PNG_FMT_RAW:
        return 0;
      default:
        return PNG_EFMT;
    }
  }

  int PngParser::iHeader::calculate_image_width(int fmt, size_t& len) const {
    size_t res = width;
    unsigned bytes_per_pixel;

    switch (fmt) {
      case PNG_FMT_RGBA8:
        bytes_per_pixel = 4;
        break;
      case PNG_FMT_RGBA16:
        bytes_per_pixel = 8;
        break;
      case PNG_FMT_RGB8:
        bytes_per_pixel = 3;
        break;
      case PNG_FMT_PNG:
      case PNG_FMT_RAW: {
        int ret = calculate_scanline_width(width, res);
        if (ret) return ret;

        res -= 1; /* exclude filter byte */
        bytes_per_pixel = 1;
        break;
      }
      default:
        return PNG_EINTERNAL;
    }

    if (res > SIZE_MAX / bytes_per_pixel) return PNG_EOVERFLOW;
    res = res * bytes_per_pixel;

    len = res;

    return 0;
  }

  int PngParser::iHeader::calculate_image_size(int fmt, size_t& len) const {
    size_t res = 0;

    int ret = calculate_image_width(fmt, res);
    if (ret) return ret;

    if (res > SIZE_MAX / height) return PNG_EOVERFLOW;
    res = res * height;

    len = res;

    return 0;
  }

  int PngParser::increase_cache_usage(size_t bytes, int new_chunk) {
    if (!bytes) return PNG_EINTERNAL;

    if (new_chunk) {
      _chunk_count_total++;
      if (_chunk_count_total < 1) return PNG_EOVERFLOW;

      if (_chunk_count_total > _chunk_count_limit) return PNG_ECHUNK_LIMITS;
    }

    size_t new_usage = _chunk_cache_usage + bytes;

    if (new_usage < _chunk_cache_usage) return PNG_EOVERFLOW;

    if (new_usage > _chunk_cache_limit) return PNG_ECHUNK_LIMITS;

    _chunk_cache_usage = new_usage;

    return 0;
  }

  int PngParser::decrease_cache_usage(size_t usage) {
    if (!usage) return PNG_EINTERNAL;
    if (usage > _chunk_cache_usage) return PNG_EINTERNAL;
    _chunk_cache_usage -= usage;
    return 0;
  }

  int PngParser::Chunk::is_critical_chunk() const {
    return ((type[0] & (1 << 5)) == 0) ? 1 : 0;
  }

  int PngParser::on_error(int err) {
    _state = PNG_STATE_INVALID;
    return err;
  }

  int PngParser::read_data(size_t bytes) {
    if (!bytes) return 0;

    if (_streaming && (bytes > PNG_READ_SIZE)) return PNG_EINTERNAL;

    int ret = _read_fn(this, _stream_user_ptr, _stream_buf, bytes);

    if (ret) {
      if (ret > 0 || ret < PNG_IO_ERROR) ret = PNG_IO_ERROR;

      return ret;
    }

    _bytes_read += bytes;
    if (_bytes_read < bytes) return PNG_EOVERFLOW;

    return 0;
  }

/* Ensure there is enough space for encoding starting at ctx->_write_ptr  */
  int PngParser::require_bytes(size_t bytes) {
    if (_streaming) {
      if (bytes > _stream_buf_size) {
        size_t new_size = _stream_buf_size;

        /* Start at default IDAT size + header + crc */
        if (new_size < (PNG_WRITE_SIZE + 12)) new_size = PNG_WRITE_SIZE + 12;

        if (new_size < bytes) new_size = bytes;

        void *temp = realloc(_stream_buf, new_size);

        if (temp == nullptr) return on_error(PNG_EMEM);

        _stream_buf = (byte *) temp;
        _stream_buf_size = bytes;
        _write_ptr = _stream_buf;
      }

      return 0;
    }

    if (!_internal_buffer) return PNG_ENODST;

    size_t required = _bytes_encoded + bytes;
    if (required < bytes) return PNG_EOVERFLOW;

    if (required > _out_png_size) {
      size_t new_size = _out_png_size;

      /* Start with a size that doesn't require a realloc() 100% of the time */
      if (new_size < (PNG_WRITE_SIZE * 2)) new_size = PNG_WRITE_SIZE * 2;

      /* Prefer the next power of two over the requested size */
      while (new_size < required) {
        if (new_size / SIZE_MAX > 2) return on_error(PNG_EOVERFLOW);

        new_size *= 2;
      }

      void *temp = realloc(_out_png, new_size);

      if (temp == nullptr) return on_error(PNG_EMEM);

      _out_png = (byte *) temp;
      _out_png_size = new_size;
      _write_ptr = _out_png + _bytes_encoded;
    }

    return 0;
  }

  int PngParser::write_data(const void *data, size_t bytes) {
    if (!bytes) return 0;

    if (_streaming) {
      if (bytes > PNG_WRITE_SIZE) return PNG_EINTERNAL;

      int ret = _write_fn(this, _stream_user_ptr, (void *) data, bytes);

      if (ret) {
        if (ret > 0 || ret < PNG_IO_ERROR) ret = PNG_IO_ERROR;

        return on_error(ret);
      }
    } else {
      int ret = require_bytes(bytes);
      if (ret) return on_error(ret);

      memcpy(_write_ptr, data, bytes);

      _write_ptr += bytes;
    }

    _bytes_encoded += bytes;
    if (_bytes_encoded < bytes) return PNG_EOVERFLOW;

    return 0;
  }

  int PngParser::write_header(const uint8_t chunk_type[4], size_t chunk_length, unsigned char **data) {
    if (chunk_type == nullptr) return PNG_EINTERNAL;
    if (chunk_length > _u32max) return PNG_EINTERNAL;

    size_t total = chunk_length + 12;

    int ret = require_bytes(total);
    if (ret) return ret;

    uint32_t crc = gccucore::zlib::crc32(0, nullptr, 0);
    _current_chunk.crc = gccucore::zlib::crc32(crc, chunk_type, 4);

    memcpy(&_current_chunk.type, chunk_type, 4);
    _current_chunk.length = (uint32_t) chunk_length;

    if (!data) return PNG_EINTERNAL;

    if (_streaming) *data = _stream_buf + 8;
    else *data = _write_ptr + 8;

    return 0;
  }

  int PngParser::trim_chunk(uint32_t length) {
    if (length > _u32max) return PNG_EINTERNAL;
    if (length > _current_chunk.length) return PNG_EINTERNAL;

    _current_chunk.length = length;

    return 0;
  }

  int PngParser::finish_chunk() {
    Chunk *chunk = &_current_chunk;

    unsigned char *header;
    unsigned char *chunk_data;

    if (_streaming) {
      chunk_data = _stream_buf + 8;
      header = _stream_buf;
    } else {
      chunk_data = _write_ptr + 8;
      header = _write_ptr;
    }

    write_u32(header, chunk->length);
    memcpy(header + 4, chunk->type, 4);

    chunk->crc = gccucore::zlib::crc32(chunk->crc, chunk_data, chunk->length);

    write_u32(chunk_data + chunk->length, chunk->crc);

    if (_streaming) {
      const unsigned char *ptr = _stream_buf;
      uint32_t bytes_left = chunk->length + 12;
      uint32_t len = 0;

      while (bytes_left) {
        ptr += len;
        len = PNG_WRITE_SIZE;

        if (len > bytes_left) len = bytes_left;

        int ret = write_data(ptr, len);
        if (ret) return ret;

        bytes_left -= len;
      }
    } else {
      _bytes_encoded += chunk->length;
      if (_bytes_encoded < chunk->length) return PNG_EOVERFLOW;

      _bytes_encoded += 12;
      if (_bytes_encoded < 12) return PNG_EOVERFLOW;

      _write_ptr += chunk->length + 12;
    }

    return 0;
  }

  int PngParser::write_chunk(const uint8_t type[4], const void *data, size_t length) {
    if (type == nullptr) return PNG_EINTERNAL;
    if (length && data == nullptr) return PNG_EINTERNAL;

    unsigned char *write_ptr;

    int ret = write_header(type, length, &write_ptr);
    if (ret) return ret;

    if (length) memcpy(write_ptr, data, length);

    return finish_chunk();
  }

  int PngParser::write_iend() {
    unsigned char iend_chunk[12] = {0, 0, 0, 0, 73, 69, 78, 68, 174, 66, 96, 130};
    return write_data(iend_chunk, 12);
  }

  int PngParser::write_unknown_chunks(Location location) {
    if (!_stored.unknown) return 0;

    const struct UnknownChunk *chunk = _chunk_list;

    uint32_t i;
    for (i = 0; i < _n_chunks; i++, chunk++) {
      if (chunk->location != location) continue;

      int ret = write_chunk(chunk->type, chunk->data, chunk->length);
      if (ret) return ret;
    }

    return 0;
  }

/* Read and check the current chunk's crc,
   returns -PNG_CRC_DISCARD if the chunk should be discarded */
  int PngParser::read_and_check_crc() {
    int ret;
    ret = read_data(4);
    if (ret) return ret;

    _current_chunk.crc = read_u32(_data);

    if (_skip_crc) return 0;

    if (_cur_actual_crc != _current_chunk.crc) {
      if (_current_chunk.is_critical_chunk()) {
        if (_crc_action_critical == PNG_CRC_USE) return 0;
      } else {
        if (_crc_action_ancillary == PNG_CRC_USE) return 0;
        if (_crc_action_ancillary == PNG_CRC_DISCARD) return -PNG_CRC_DISCARD;
      }

      return PNG_ECHUNK_CRC;
    }

    return 0;
  }

/* Read and validate the current chunk's crc and the next chunk header */
  int PngParser::read_header() {
    int ret;
    Chunk chunk = {0};

    ret = read_and_check_crc();
    if (ret) {
      if (ret == -PNG_CRC_DISCARD) {
        _discard = 1;
      } else return ret;
    }

    ret = read_data(8);
    if (ret) return ret;

    chunk.offset = _bytes_read - 8;

    chunk.length = read_u32(_data);

    memcpy(&chunk.type, _data + 4, 4);

    if (chunk.length > _u32max) return PNG_ECHUNK_STDLEN;

    _cur_chunk_bytes_left = chunk.length;

    if (chunk.is_critical_chunk() && _crc_action_critical == PNG_CRC_USE) _skip_crc = 1;
    else if (_crc_action_ancillary == PNG_CRC_USE) _skip_crc = 1;
    else _skip_crc = 0;

    if (!_skip_crc) {
      _cur_actual_crc = gccucore::zlib::crc32(0, nullptr, 0);
      _cur_actual_crc = gccucore::zlib::crc32(_cur_actual_crc, chunk.type, 4);
    }

    _current_chunk = chunk;

    return 0;
  }

/* Read chunk bytes and update crc */
  int PngParser::read_chunk_bytes(uint32_t bytes) {
    if (!_cur_chunk_bytes_left || !bytes) return PNG_EINTERNAL;
    if (bytes > _cur_chunk_bytes_left) return PNG_EINTERNAL; /* XXX: more specific error? */

    int ret;

    ret = read_data(bytes);
    if (ret) return ret;

    if (!_skip_crc) _cur_actual_crc = gccucore::zlib::crc32(_cur_actual_crc, _data, bytes);

    _cur_chunk_bytes_left -= bytes;

    return ret;
  }

/* read_chunk_bytes() + read_data() with custom output buffer */
  int PngParser::read_chunk_bytes2(void *out, uint32_t bytes) {
    if (!_cur_chunk_bytes_left || !bytes) return PNG_EINTERNAL;
    if (bytes > _cur_chunk_bytes_left) return PNG_EINTERNAL; /* XXX: more specific error? */

    int ret;
    uint32_t len = bytes;

    if (_streaming && len > PNG_READ_SIZE) len = PNG_READ_SIZE;

    while (bytes) {
      if (len > bytes) len = bytes;

      ret = _read_fn(this, _stream_user_ptr, out, len);
      if (ret) return ret;

      if (!_streaming) memcpy(out, _data, len);

      _bytes_read += len;
      if (_bytes_read < len) return PNG_EOVERFLOW;

      if (!_skip_crc) _cur_actual_crc = gccucore::zlib::crc32(_cur_actual_crc, (const byte *) out, len);

      _cur_chunk_bytes_left -= len;

      out = (char *) out + len;
      bytes -= len;
      len = PNG_READ_SIZE;
    }

    return 0;
  }

  int PngParser::discard_chunk_bytes(uint32_t bytes) {
    if (!bytes) return 0;

    int ret;

    if (_streaming) /* Do small, consecutive reads */
    {
      while (bytes) {
        uint32_t len = PNG_READ_SIZE;

        if (len > bytes) len = bytes;

        ret = read_chunk_bytes(len);
        if (ret) return ret;

        bytes -= len;
      }
    } else {
      ret = read_chunk_bytes(bytes);
      if (ret) return ret;
    }

    return 0;
  }

  int PngParser::inflate_init(int window_bits) {
    if (_zstream.state) gccucore::zlib::inflateEnd(&_zstream);

    _inflate = 1;

    _zstream.zalloc = _zalloc;
    _zstream.zfree = _zfree;
    _zstream.opaque = this;

    if (gccucore::zlib::inflateInit2(&_zstream, window_bits) != Z_OK) return PNG_EZLIB_INIT;

    return 0;
  }

  int PngParser::deflate_init(const ZLIBOptions &options) {
    if (_zstream.state) gccucore::zlib::deflateEnd(&_zstream);

    _deflate = 1;

    gccucore::zlib::z_stream *zstream = &_zstream;
    zstream->zalloc = _zalloc;
    zstream->zfree = _zfree;
    zstream->opaque = this;
    zstream->data_type = options._dataType;

    int ret = gccucore::zlib::deflateInit2(zstream, options._compressionLevel, Z_DEFLATED, options._windowBits,
                                           options._memLevel, options._strategy);

    if (ret != Z_OK) return PNG_EZLIB_INIT;

    return 0;
  }

/* Inflate a zlib stream starting with start_buf if non-nullptr,
   continuing from the datastream till an end marker,
   allocating and writing the inflated stream to *out,
   leaving "extra" bytes at the end, final buffer length is *len.

   Takes into account the chunk size and cache limits.
*/
  int PngParser::inflate_stream(char **out, size_t *len, size_t extra, const void *start_buf, size_t start_len) {
    int ret = inflate_init(15);
    if (ret) return ret;

    size_t max = _chunk_cache_limit - _chunk_cache_usage;

    if (_max_chunk_size < max) max = _max_chunk_size;

    if (extra > max) return PNG_ECHUNK_LIMITS;
    max -= extra;

    uint32_t read_size;
    size_t size = 8 * 1024;
    void *t, *buf = malloc(size);

    if (buf == nullptr) return PNG_EMEM;

    gccucore::zlib::z_stream *stream = &_zstream;

    if (start_buf != nullptr && start_len) {
      stream->avail_in = (uint32_t) start_len;
      stream->next_in = (const byte *) start_buf;
    } else {
      stream->avail_in = 0;
      stream->next_in = nullptr;
    }

    stream->avail_out = (uint32_t) size;
    stream->next_out = (byte *) buf;

    while (ret != Z_STREAM_END) {
      ret = inflate(stream, Z_NO_FLUSH);

      if (ret == Z_STREAM_END) break;

      if (ret != Z_OK && ret != Z_BUF_ERROR) {
        ret = PNG_EZLIB;
        goto err;
      }

      if (!stream->avail_out) /* Resize buffer */
      {
        /* overflow or reached chunk/cache limit */
        if ((2 > SIZE_MAX / size) || (size > max / 2)) {
          ret = PNG_ECHUNK_LIMITS;
          goto err;
        }

        size *= 2;

        t = realloc(buf, size);
        if (t == nullptr) goto mem;

        buf = t;

        stream->avail_out = (uint32_t) size / 2;
        stream->next_out = (unsigned char *) buf + size / 2;
      } else if (!stream->avail_in) /* Read more chunk bytes */
      {
        read_size = _cur_chunk_bytes_left;
        if (_streaming && read_size > PNG_READ_SIZE) read_size = PNG_READ_SIZE;

        ret = read_chunk_bytes(read_size);

        if (ret) {
          if (!read_size) ret = PNG_EZLIB;

          goto err;
        }

        stream->avail_in = read_size;
        stream->next_in = _data;
      }
    }

    size = stream->total_out;

    if (!size) {
      ret = PNG_EZLIB;
      goto err;
    }

    size += extra;
    if (size < extra) goto mem;

    t = realloc(buf, size);
    if (t == nullptr) goto mem;

    buf = t;

    (void) increase_cache_usage(size, 0);

    *out = (char *) buf;
    *len = size;

    return 0;

    mem:
    ret = PNG_EMEM;
    err:
    free(buf);
    return ret;
  }

/* Read at least one byte from the IDAT stream */
  int PngParser::read_idat_bytes(uint32_t *bytes_read) {
    if (bytes_read == nullptr) return PNG_EINTERNAL;
    if (memcmp(_current_chunk.type, type_idat, 4)) return PNG_EIDAT_TOO_SHORT;

    int ret;
    uint32_t len;

    while (!_cur_chunk_bytes_left) {
      ret = read_header();
      if (ret) return ret;

      if (memcmp(_current_chunk.type, type_idat, 4)) return PNG_EIDAT_TOO_SHORT;
    }

    if (_streaming) {/* TODO: estimate bytes to read for progressive reads */
      len = PNG_READ_SIZE;
      if (len > _cur_chunk_bytes_left) len = _cur_chunk_bytes_left;
    } else len = _current_chunk.length;

    ret = read_chunk_bytes(len);

    *bytes_read = len;

    return ret;
  }

  int PngParser::read_scanline_bytes(unsigned char *dest, size_t len) {
    if (dest == nullptr) return PNG_EINTERNAL;

    int ret = Z_OK;
    uint32_t bytes_read;

    gccucore::zlib::z_stream *zstream = &_zstream;

    zstream->avail_out = (uint32_t) len;
    zstream->next_out = dest;

    while (zstream->avail_out != 0) {
      ret = inflate(zstream, Z_NO_FLUSH);

      if (ret == Z_OK) continue;

      if (ret == Z_STREAM_END) /* Reached an end-marker */
      {
        if (zstream->avail_out != 0) return PNG_EIDAT_TOO_SHORT;
      } else if (ret == Z_BUF_ERROR) /* Read more IDAT bytes */
      {
        ret = read_idat_bytes(&bytes_read);
        if (ret) return ret;

        zstream->avail_in = bytes_read;
        zstream->next_in = _data;
      } else return PNG_EIDAT_STREAM;
    }

    return 0;
  }

  static uint8_t paeth(uint8_t a, uint8_t b, uint8_t c) {
    int16_t p = a + b - c;
    int16_t pa = abs(p - a);
    int16_t pb = abs(p - b);
    int16_t pc = abs(p - c);

    if (pa <= pb && pa <= pc) return a;
    else if (pb <= pc) return b;

    return c;
  }

  //PNG_TARGET_CLONES("default,avx2")

  static void defilter_up(size_t bytes, unsigned char *row, const unsigned char *prev) {
    size_t i;
    for (i = 0; i < bytes; i++) {
      row[i] += prev[i];
    }
  }

/* Defilter *scanline in-place.
   *prev_scanline and *scanline should point to the first pixel,
   scanline_width is the width of the scanline including the filter byte.
*/
  int PngParser::defilter_scanline(const unsigned char *prev_scanline, unsigned char *scanline,
                                   size_t scanline_width, unsigned bytes_per_pixel, const unsigned filter) {
    if (prev_scanline == nullptr || scanline == nullptr || !scanline_width) return PNG_EINTERNAL;

    size_t i;
    scanline_width--;

    if (filter == 0) return 0;

    if (filter == PNG_FILTER_UP) {
      defilter_up(scanline_width, scanline, prev_scanline);
      return 0;
    }

    if (bytes_per_pixel == 4) {
      if (filter == PNG_FILTER_SUB)
        defilter_sub4(scanline_width, scanline);
      else if (filter == PNG_FILTER_AVERAGE)
        defilter_avg4(scanline_width, scanline, prev_scanline);
      else if (filter == PNG_FILTER_PAETH)
        defilter_paeth4(scanline_width, scanline, prev_scanline);
      else return PNG_EFILTER;

      return 0;
    } else if (bytes_per_pixel == 3) {
      if (filter == PNG_FILTER_SUB)
        defilter_sub3(scanline_width, scanline);
      else if (filter == PNG_FILTER_AVERAGE)
        defilter_avg3(scanline_width, scanline, prev_scanline);
      else if (filter == PNG_FILTER_PAETH)
        defilter_paeth3(scanline_width, scanline, prev_scanline);
      else return PNG_EFILTER;

      return 0;
    }

    for (i = 0; i < scanline_width; i++) {
      uint8_t x, a, b, c;

      if (i >= bytes_per_pixel) {
        a = scanline[i - bytes_per_pixel];
        b = prev_scanline[i];
        c = prev_scanline[i - bytes_per_pixel];
      } else /* First pixel in row */
      {
        a = 0;
        b = prev_scanline[i];
        c = 0;
      }

      x = scanline[i];

      switch (filter) {
        case PNG_FILTER_SUB: {
          x = x + a;
          break;
        }
        case PNG_FILTER_AVERAGE: {
          uint16_t avg = (a + b) / 2;
          x = x + avg;
          break;
        }
        case PNG_FILTER_PAETH: {
          x = x + paeth(a, b, c);
          break;
        }
      }

      scanline[i] = x;
    }

    return 0;
  }

  int PngParser::filter_scanline(unsigned char *filtered, const unsigned char *prev_scanline, const unsigned char *scanline,
                                 size_t scanline_width, unsigned bytes_per_pixel, const unsigned filter) {
    if (prev_scanline == nullptr || scanline == nullptr || scanline_width <= 1) return PNG_EINTERNAL;

    if (filter > 4) return PNG_EFILTER;
    if (filter == 0) return 0;

    scanline_width--;

    uint32_t i;
    for (i = 0; i < scanline_width; i++) {
      uint8_t x, a, b, c;

      if (i >= bytes_per_pixel) {
        a = scanline[i - bytes_per_pixel];
        b = prev_scanline[i];
        c = prev_scanline[i - bytes_per_pixel];
      } else /* first pixel in row */
      {
        a = 0;
        b = prev_scanline[i];
        c = 0;
      }

      x = scanline[i];

      switch (filter) {
        case PNG_FILTER_SUB: {
          x = x - a;
          break;
        }
        case PNG_FILTER_UP: {
          x = x - b;
          break;
        }
        case PNG_FILTER_AVERAGE: {
          uint16_t avg = (a + b) / 2;
          x = x - avg;
          break;
        }
        case PNG_FILTER_PAETH: {
          x = x - paeth(a, b, c);
          break;
        }
      }

      filtered[i] = x;
    }

    return 0;
  }

  int PngParser::filter_sum(const unsigned char *prev_scanline, const unsigned char *scanline,
                                size_t size, unsigned bytes_per_pixel, const unsigned filter) {
    /* prevent potential over/underflow, bails out at a width of ~8M pixels for RGBA8 */
    if (size > (INT32_MAX / 128)) return INT32_MAX;

    uint32_t i;
    int32_t sum = 0;
    uint8_t x, a, b, c;

    for (i = 0; i < size; i++) {
      if (i >= bytes_per_pixel) {
        a = scanline[i - bytes_per_pixel];
        b = prev_scanline[i];
        c = prev_scanline[i - bytes_per_pixel];
      } else /* first pixel in row */
      {
        a = 0;
        b = prev_scanline[i];
        c = 0;
      }

      x = scanline[i];

      switch (filter) {
        case PNG_FILTER_NONE: {
          break;
        }
        case PNG_FILTER_SUB: {
          x = x - a;
          break;
        }
        case PNG_FILTER_UP: {
          x = x - b;
          break;
        }
        case PNG_FILTER_AVERAGE: {
          uint16_t avg = (a + b) / 2;
          x = x - avg;
          break;
        }
        case PNG_FILTER_PAETH: {
          x = x - paeth(a, b, c);
          break;
        }
      }

      sum += 128 - abs((int) x - 128);
    }

    return sum;
  }

  unsigned PngParser::get_best_filter(const unsigned char *prev_scanline, const unsigned char *scanline,
                                      size_t scanline_width, unsigned bytes_per_pixel, const int choices) {
    if (!choices) return PNG_FILTER_NONE;

    scanline_width--;

    int i;
    unsigned int best_filter = 0;
    FilterChoice flag;
    int32_t sum, best_score = INT32_MAX;
    int32_t filter_scores[5] = {INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX};

    if (!(choices & (choices - 1))) {/* only one choice/bit is set */
      for (i = 0; i < 5; i++) {
        if (choices == 1 << (i + 3)) return i;
      }
    }

    for (i = 0; i < 5; i++) {
      flag = (FilterChoice) (1 << (i + 3));

      if (choices & flag) sum = filter_sum(prev_scanline, scanline, scanline_width, bytes_per_pixel, i);
      else continue;

      filter_scores[i] = abs(sum);

      if (filter_scores[i] < best_score) {
        best_score = filter_scores[i];
        best_filter = i;
      }
    }

    return best_filter;
  }

/* Scale "sbits" significant bits in "sample" from "bit_depth" to "target"

   "bit_depth" must be a valid PNG depth
   "sbits" must be less than or equal to "bit_depth"
   "target" must be between 1 and 16
*/
  static uint16_t sample_to_target(uint16_t sample, unsigned bit_depth, unsigned sbits, unsigned target) {
    if (bit_depth == sbits) {
      if (target == sbits) return sample; /* No scaling */
    }/* bit_depth > sbits */
    else sample = sample >> (bit_depth - sbits); /* Shift significant bits to bottom */

    /* Downscale */
    if (target < sbits) return sample >> (sbits - target);

    /* Upscale using left bit replication */
    int8_t shift_amount = target - sbits;
    uint16_t sample_bits = sample;
    sample = 0;

    while (shift_amount >= 0) {
      sample = sample | (sample_bits << shift_amount);
      shift_amount -= sbits;
    }

    int8_t partial = shift_amount + (int8_t) sbits;

    if (partial != 0) sample = sample | (sample_bits >> abs(shift_amount));

    return sample;
  }

  void PngParser::gamma_correct_row(unsigned char *row, uint32_t pixels, int fmt, const uint16_t *gamma_lut) {
    uint32_t i;

    if (fmt == PNG_FMT_RGBA8) {
      unsigned char *px;
      for (i = 0; i < pixels; i++) {
        px = row + i * 4;

        px[0] = gamma_lut[px[0]];
        px[1] = gamma_lut[px[1]];
        px[2] = gamma_lut[px[2]];
      }
    } else if (fmt == PNG_FMT_RGBA16) {
      for (i = 0; i < pixels; i++) {
        uint16_t px[4];
        memcpy(px, row + i * 8, 8);

        px[0] = gamma_lut[px[0]];
        px[1] = gamma_lut[px[1]];
        px[2] = gamma_lut[px[2]];

        memcpy(row + i * 8, px, 8);
      }
    } else if (fmt == PNG_FMT_RGB8) {
      unsigned char *px;
      for (i = 0; i < pixels; i++) {
        px = row + i * 3;

        px[0] = gamma_lut[px[0]];
        px[1] = gamma_lut[px[1]];
        px[2] = gamma_lut[px[2]];
      }
    }
  }

/* Apply transparency to output row */
  void PngParser::trns_row(unsigned char *row, const unsigned char *scanline, const unsigned char *trns,
                           unsigned scanline_stride, iHeader *ihdr, uint32_t pixels, int fmt) {
    uint32_t i;
    unsigned row_stride;

    if (fmt == PNG_FMT_RGBA8) {
      if (ihdr->color_type == PNG_COLOR_TYPE_GRAYSCALE) return; /* already applied in the decoding loop */

      row_stride = 4;
      for (i = 0; i < pixels; i++, scanline += scanline_stride, row += row_stride) {
        if (!memcmp(scanline, trns, scanline_stride)) row[3] = 0;
      }
    } else if (fmt == PNG_FMT_RGBA16) {
      if (ihdr->color_type == PNG_COLOR_TYPE_GRAYSCALE) return; /* already applied in the decoding loop */

      row_stride = 8;
      for (i = 0; i < pixels; i++, scanline += scanline_stride, row += row_stride) {
        if (!memcmp(scanline, trns, scanline_stride)) memset(row + 6, 0, 2);
      }
    } else return;
  }

  void PngParser::scale_row(unsigned char *row, uint32_t pixels, int fmt, unsigned depth, const struct SBIT *sbit) {
    uint32_t i;

    if (fmt == PNG_FMT_RGBA8) {
      unsigned char px[4];
      for (i = 0; i < pixels; i++) {
        memcpy(px, row + i * 4, 4);

        px[0] = sample_to_target(px[0], depth, sbit->red_bits, 8);
        px[1] = sample_to_target(px[1], depth, sbit->green_bits, 8);
        px[2] = sample_to_target(px[2], depth, sbit->blue_bits, 8);
        px[3] = sample_to_target(px[3], depth, sbit->alpha_bits, 8);

        memcpy(row + i * 4, px, 4);
      }
    } else if (fmt == PNG_FMT_RGBA16) {
      uint16_t px[4];
      for (i = 0; i < pixels; i++) {
        memcpy(px, row + i * 8, 8);

        px[0] = sample_to_target(px[0], depth, sbit->red_bits, 16);
        px[1] = sample_to_target(px[1], depth, sbit->green_bits, 16);
        px[2] = sample_to_target(px[2], depth, sbit->blue_bits, 16);
        px[3] = sample_to_target(px[3], depth, sbit->alpha_bits, 16);

        memcpy(row + i * 8, px, 8);
      }
    } else if (fmt == PNG_FMT_RGB8) {
      unsigned char px[4];
      for (i = 0; i < pixels; i++) {
        memcpy(px, row + i * 3, 3);

        px[0] = sample_to_target(px[0], depth, sbit->red_bits, 8);
        px[1] = sample_to_target(px[1], depth, sbit->green_bits, 8);
        px[2] = sample_to_target(px[2], depth, sbit->blue_bits, 8);

        memcpy(row + i * 3, px, 3);
      }
    }
  }

/* Expand to *row using 8-bit palette indices from *scanline */
  void PngParser::expand_row(unsigned char *row, const unsigned char *scanline, const union DecodePLTE *decode_plte,
                         uint32_t width, int fmt) {
    uint32_t i = 0;
    unsigned char *px;
    unsigned char entry;
    const struct PLTEEntry *plte = decode_plte->rgba;

    if (fmt == PNG_FMT_RGBA8) {
      for (; i < width; i++) {
        px = row + i * 4;
        entry = scanline[i];
        px[0] = plte[entry].red;
        px[1] = plte[entry].green;
        px[2] = plte[entry].blue;
        px[3] = plte[entry].alpha;
      }
    } else if (fmt == PNG_FMT_RGB8) {
      for (; i < width; i++) {
        px = row + i * 3;
        entry = scanline[i];
        px[0] = plte[entry].red;
        px[1] = plte[entry].green;
        px[2] = plte[entry].blue;
      }
    }
  }

/* Unpack 1/2/4/8-bit samples to G8/GA8/GA16 or G16 -> GA16 */
  void PngParser::unpack_scanline(unsigned char *out, const unsigned char *scanline, uint32_t width, unsigned bit_depth, int fmt) {
    PngParser::_iter iter(bit_depth, scanline);
    uint32_t i;

    /* 1/2/4-bit -> 8-bit */
    for (i = 0; i < width; i++) {
      out[i] = iter.get_sample();
    }
  }

  int PngParser::check_ihdr(const iHeader &ihdr, uint32_t max_width, uint32_t max_height) {
    if (ihdr.width > _u32max || !ihdr.width) return PNG_EWIDTH;
    if (ihdr.height > _u32max || !ihdr.height) return PNG_EHEIGHT;

    if (ihdr.width > max_width) return PNG_EUSER_WIDTH;
    if (ihdr.height > max_height) return PNG_EUSER_HEIGHT;

    switch (ihdr.color_type) {
      case PNG_COLOR_TYPE_GRAYSCALE: {
        if (!(ihdr.bit_depth == 1 || ihdr.bit_depth == 2 ||
              ihdr.bit_depth == 4 || ihdr.bit_depth == 8 ||
              ihdr.bit_depth == 16))
          return PNG_EBIT_DEPTH;

        break;
      }
      case PNG_COLOR_TYPE_TRUECOLOR:
      case PNG_COLOR_TYPE_GRAYSCALE_ALPHA:
      case PNG_COLOR_TYPE_TRUECOLOR_ALPHA: {
        if (!(ihdr.bit_depth == 8 || ihdr.bit_depth == 16))
          return PNG_EBIT_DEPTH;

        break;
      }
      case PNG_COLOR_TYPE_INDEXED: {
        if (!(ihdr.bit_depth == 1 || ihdr.bit_depth == 2 ||
              ihdr.bit_depth == 4 || ihdr.bit_depth == 8))
          return PNG_EBIT_DEPTH;

        break;
      }
      default:
        return PNG_ECOLOR_TYPE;
    }

    if (ihdr.compression_method) return PNG_ECOMPRESSION_METHOD;
    if (ihdr.filter_method) return PNG_EFILTER_METHOD;

    if (ihdr.interlace_method > 1) return PNG_EINTERLACE_METHOD;

    return 0;
  }

  int PngParser::check_plte(const struct PLTE &plte, const iHeader &ihdr) {
    if (plte.n_entries == 0) return 1;
    if (plte.n_entries > 256) return 1;

    if (ihdr.color_type == PNG_COLOR_TYPE_INDEXED) {
      if (plte.n_entries > (1U << ihdr.bit_depth)) return 1;
    }

    return 0;
  }

  int PngParser::check_sbit(const struct SBIT &sbit, const iHeader &ihdr) {
    if (ihdr.color_type == 0) {
      if (sbit.grayscale_bits == 0) return PNG_ESBIT;
      if (sbit.grayscale_bits > ihdr.bit_depth) return PNG_ESBIT;
    } else if (ihdr.color_type == 2 || ihdr.color_type == 3) {
      if (sbit.red_bits == 0) return PNG_ESBIT;
      if (sbit.green_bits == 0) return PNG_ESBIT;
      if (sbit.blue_bits == 0) return PNG_ESBIT;

      uint8_t bit_depth;
      if (ihdr.color_type == 3) bit_depth = 8;
      else bit_depth = ihdr.bit_depth;

      if (sbit.red_bits > bit_depth) return PNG_ESBIT;
      if (sbit.green_bits > bit_depth) return PNG_ESBIT;
      if (sbit.blue_bits > bit_depth) return PNG_ESBIT;
    } else if (ihdr.color_type == 4) {
      if (sbit.grayscale_bits == 0) return PNG_ESBIT;
      if (sbit.alpha_bits == 0) return PNG_ESBIT;

      if (sbit.grayscale_bits > ihdr.bit_depth) return PNG_ESBIT;
      if (sbit.alpha_bits > ihdr.bit_depth) return PNG_ESBIT;
    } else if (ihdr.color_type == 6) {
      if (sbit.red_bits == 0) return PNG_ESBIT;
      if (sbit.green_bits == 0) return PNG_ESBIT;
      if (sbit.blue_bits == 0) return PNG_ESBIT;
      if (sbit.alpha_bits == 0) return PNG_ESBIT;

      if (sbit.red_bits > ihdr.bit_depth) return PNG_ESBIT;
      if (sbit.green_bits > ihdr.bit_depth) return PNG_ESBIT;
      if (sbit.blue_bits > ihdr.bit_depth) return PNG_ESBIT;
      if (sbit.alpha_bits > ihdr.bit_depth) return PNG_ESBIT;
    }

    return 0;
  }

  int PngParser::check_chrm_int(const struct ChrmInt &chrm_int) {
    if (chrm_int.white_point_x > _u32max ||
        chrm_int.white_point_y > _u32max ||
        chrm_int.red_x > _u32max ||
        chrm_int.red_y > _u32max ||
        chrm_int.green_x > _u32max ||
        chrm_int.green_y > _u32max ||
        chrm_int.blue_x > _u32max ||
        chrm_int.blue_y > _u32max)
      return PNG_ECHRM;

    return 0;
  }

  int PngParser::check_phys(const struct PHYS &phys) {
    if (phys.unit_specifier > 1) return PNG_EPHYS;

    if (phys.ppu_x > _u32max) return PNG_EPHYS;
    if (phys.ppu_y > _u32max) return PNG_EPHYS;

    return 0;
  }

  int PngParser::check_time(const struct Time &time) {
    if (time.month == 0 || time.month > 12) return 1;
    if (time.day == 0 || time.day > 31) return 1;
    if (time.hour > 23) return 1;
    if (time.minute > 59) return 1;
    if (time.second > 60) return 1;

    return 0;
  }

  int PngParser::check_offs(const struct Offs &offs) {
    return offs.unit_specifier > 1 ? 1 : 0;
  }

  int PngParser::check_exif(const struct Exif &exif) {
    if (exif.data == nullptr) return 1;

    if (exif.length < 4) return PNG_ECHUNK_SIZE;
    if (exif.length > _u32max) return PNG_ECHUNK_STDLEN;

    const uint8_t exif_le[4] = {73, 73, 42, 0};
    const uint8_t exif_be[4] = {77, 77, 0, 42};

    if (memcmp(exif.data, exif_le, 4) && memcmp(exif.data, exif_be, 4)) return 1;

    return 0;
  }

/* Validate PNG keyword */
  int PngParser::check_png_keyword(const char *str) {
    if (str == nullptr) return 1;
    size_t len = strlen(str);
    const char *end = str + len;

    if (!len) return 1;
    if (len > 79) return 1;
    if (str[0] == ' ') return 1; /* Leading space */
    if (end[-1] == ' ') return 1; /* Trailing space */
    if (strstr(str, "  ") != nullptr) return 1; /* Consecutive spaces */

    uint8_t c;
    while (str != end) {
      memcpy(&c, str, 1);

      if ((c >= 32 && c <= 126) || (c >= 161)) str++;
      else return 1; /* Invalid character */
    }

    return 0;
  }

/* Validate PNG text *str up to 'len' bytes */
  static int check_png_text(const char *str, size_t len) {/* XXX: are consecutive newlines permitted? */
    if (str == nullptr || len == 0) return 1;

    uint8_t c;
    size_t i = 0;
    while (i < len) {
      memcpy(&c, str + i, 1);

      if ((c >= 32 && c <= 126) || (c >= 161) || c == 10) i++;
      else return 1; /* Invalid character */
    }

    return 0;
  }

/* Returns non-zero for standard chunks which are stored without allocating memory */
  static int is_small_chunk(uint8_t type[4]) {
    if (!memcmp(type, type_plte, 4)) return 1;
    else if (!memcmp(type, type_chrm, 4)) return 1;
    else if (!memcmp(type, type_gama, 4)) return 1;
    else if (!memcmp(type, type_sbit, 4)) return 1;
    else if (!memcmp(type, type_srgb, 4)) return 1;
    else if (!memcmp(type, type_bkgd, 4)) return 1;
    else if (!memcmp(type, type_trns, 4)) return 1;
    else if (!memcmp(type, type_hist, 4)) return 1;
    else if (!memcmp(type, type_phys, 4)) return 1;
    else if (!memcmp(type, type_time, 4)) return 1;
    else if (!memcmp(type, type_offs, 4)) return 1;
    else return 0;
  }

  void PngParser::read_ihdr() {
    Chunk *chunk = &_current_chunk;
    const unsigned char *data;

    chunk->offset = 8;
    chunk->length = 13;
    size_t sizeof_sig_ihdr = 29;

    int ret = read_data(sizeof_sig_ihdr);
    if (ret) {
      throw upan::exception(XLOC, _strerror(ret));
    }

    data = _data;

    if (memcmp(data, png_signature, sizeof(png_signature))) {
      throw upan::exception(XLOC, _strerror(PNG_ESIGNATURE));
    }

    chunk->length = read_u32(data + 8);
    memcpy(&chunk->type, data + 12, 4);

    if (chunk->length != 13) {
      throw upan::exception(XLOC, _strerror(PNG_EIHDR_SIZE));
    }
    if (memcmp(chunk->type, type_ihdr, 4)) {
      throw upan::exception(XLOC, _strerror(PNG_ENOIHDR));
    }

    _cur_actual_crc = gccucore::zlib::crc32(0, nullptr, 0);
    _cur_actual_crc = gccucore::zlib::crc32(_cur_actual_crc, data + 12, 17);

    _ihdr.width = read_u32(data + 16);
    _ihdr.height = read_u32(data + 20);
    _ihdr.bit_depth = data[24];
    _ihdr.color_type = data[25];
    _ihdr.compression_method = data[26];
    _ihdr.filter_method = data[27];
    _ihdr.interlace_method = data[28];

    ret = check_ihdr(_ihdr, _max_width, _max_height);
    if (ret) {
      throw upan::exception(XLOC, _strerror(ret));
    }

    _file.ihdr = 1;
    _stored.ihdr = 1;

    if (_ihdr.bit_depth < 8) _bytes_per_pixel = 1;
    else _bytes_per_pixel = _ihdr.num_channels() * (_ihdr.bit_depth / 8);

    ret = calculate_subimages();
    if (ret) {
      throw upan::exception(XLOC, _strerror(ret));
    }
  }

  void PngParser::splt_undo() {
    struct SPLT *splt = &_splt_list[_n_splt - 1];

    free(splt->entries);

    decrease_cache_usage(sizeof(struct SPLT));
    decrease_cache_usage(splt->n_entries * sizeof(struct SPLTEntry));

    splt->entries = nullptr;

    _n_splt--;
  }

  void PngParser::text_undo() {
    struct Text2 *text = &_text_list[_n_text - 1];

    free(text->keyword);
    if (text->compression_flag) free(text->text);

    decrease_cache_usage(text->cache_usage);
    decrease_cache_usage(sizeof(struct Text2));

    text->keyword = nullptr;
    text->text = nullptr;

    _n_text--;
  }

  void PngParser::chunk_undo() {
    struct UnknownChunk *chunk = &_chunk_list[_n_chunks - 1];

    free(chunk->data);

    decrease_cache_usage(chunk->length);
    decrease_cache_usage(sizeof(struct UnknownChunk));

    chunk->data = nullptr;

    _n_chunks--;
  }

  int PngParser::read_non_idat_chunks() {
    int ret;
    Chunk chunk;
    const unsigned char *data;

    _discard = 0;
    _undo = nullptr;
    _prev_stored = _stored;

    while (!(ret = read_header())) {
      if (_discard) {
        if (_undo) (this->*_undo)();

        _stored = _prev_stored;
      }

      _discard = 0;
      _undo = nullptr;

      _prev_stored = _stored;
      chunk = _current_chunk;

      if (!memcmp(chunk.type, type_idat, 4)) {
        if (_state < PNG_STATE_FIRST_IDAT) {
          if (_ihdr.color_type == 3 && !_stored.plte) return PNG_ENOPLTE;

          _first_idat = chunk;
          return 0;
        }

        if (_prev_was_idat) {
          /* Ignore extra IDAT's */
          ret = discard_chunk_bytes(chunk.length);
          if (ret) return ret;

          continue;
        } else return PNG_ECHUNK_POS; /* IDAT chunk not at the end of the IDAT sequence */
      }

      _prev_was_idat = 0;

      if (is_small_chunk(chunk.type)) {
        /* None of the known chunks can be zero length */
        if (!chunk.length) return PNG_ECHUNK_SIZE;

        /* The largest of these chunks is PLTE with 256 entries */
        ret = read_chunk_bytes(chunk.length > 768 ? 768 : chunk.length);
        if (ret) return ret;
      }

      data = _data;

      if (chunk.is_critical_chunk()) {
        if (!memcmp(chunk.type, type_plte, 4)) {
          if (_file.trns || _file.hist || _file.bkgd) return PNG_ECHUNK_POS;
          if (chunk.length % 3 != 0) return PNG_ECHUNK_SIZE;

          _plte.n_entries = chunk.length / 3;

          if (check_plte(_plte, _ihdr)) return PNG_ECHUNK_SIZE; /* XXX: EPLTE? */

          size_t i;
          for (i = 0; i < _plte.n_entries; i++) {
            _plte.entries[i].red = data[i * 3];
            _plte.entries[i].green = data[i * 3 + 1];
            _plte.entries[i].blue = data[i * 3 + 2];
          }

          _file.plte = 1;
          _stored.plte = 1;
        } else if (!memcmp(chunk.type, type_iend, 4)) {
          if (_state == PNG_STATE_AFTER_IDAT) {
            if (chunk.length) return PNG_ECHUNK_SIZE;

            ret = read_and_check_crc();
            if (ret == -PNG_CRC_DISCARD) ret = 0;

            return ret;
          } else return PNG_ECHUNK_POS;
        } else if (!memcmp(chunk.type, type_ihdr, 4)) return PNG_ECHUNK_POS;
        else return PNG_ECHUNK_UNKNOWN_CRITICAL;
      } else if (!memcmp(chunk.type, type_chrm, 4)) /* Ancillary chunks */
      {
        if (_file.plte) return PNG_ECHUNK_POS;
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.chrm) return PNG_EDUP_CHRM;

        if (chunk.length != 32) return PNG_ECHUNK_SIZE;

        _chrm_int.white_point_x = read_u32(data);
        _chrm_int.white_point_y = read_u32(data + 4);
        _chrm_int.red_x = read_u32(data + 8);
        _chrm_int.red_y = read_u32(data + 12);
        _chrm_int.green_x = read_u32(data + 16);
        _chrm_int.green_y = read_u32(data + 20);
        _chrm_int.blue_x = read_u32(data + 24);
        _chrm_int.blue_y = read_u32(data + 28);

        if (check_chrm_int(_chrm_int)) return PNG_ECHRM;

        _file.chrm = 1;
        _stored.chrm = 1;
      } else if (!memcmp(chunk.type, type_gama, 4)) {
        if (_file.plte) return PNG_ECHUNK_POS;
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.gama) return PNG_EDUP_GAMA;

        if (chunk.length != 4) return PNG_ECHUNK_SIZE;

        _gama = read_u32(data);

        if (!_gama) return PNG_EGAMA;
        if (_gama > _u32max) return PNG_EGAMA;

        _file.gama = 1;
        _stored.gama = 1;
      } else if (!memcmp(chunk.type, type_sbit, 4)) {
        if (_file.plte) return PNG_ECHUNK_POS;
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.sbit) return PNG_EDUP_SBIT;

        if (_ihdr.color_type == 0) {
          if (chunk.length != 1) return PNG_ECHUNK_SIZE;

          _sbit.grayscale_bits = data[0];
        } else if (_ihdr.color_type == 2 || _ihdr.color_type == 3) {
          if (chunk.length != 3) return PNG_ECHUNK_SIZE;

          _sbit.red_bits = data[0];
          _sbit.green_bits = data[1];
          _sbit.blue_bits = data[2];
        } else if (_ihdr.color_type == 4) {
          if (chunk.length != 2) return PNG_ECHUNK_SIZE;

          _sbit.grayscale_bits = data[0];
          _sbit.alpha_bits = data[1];
        } else if (_ihdr.color_type == 6) {
          if (chunk.length != 4) return PNG_ECHUNK_SIZE;

          _sbit.red_bits = data[0];
          _sbit.green_bits = data[1];
          _sbit.blue_bits = data[2];
          _sbit.alpha_bits = data[3];
        }

        if (check_sbit(_sbit, _ihdr)) return PNG_ESBIT;

        _file.sbit = 1;
        _stored.sbit = 1;
      } else if (!memcmp(chunk.type, type_srgb, 4)) {
        if (_file.plte) return PNG_ECHUNK_POS;
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.srgb) return PNG_EDUP_SRGB;

        if (chunk.length != 1) return PNG_ECHUNK_SIZE;

        _srgb_rendering_intent = data[0];

        if (_srgb_rendering_intent > 3) return PNG_ESRGB;

        _file.srgb = 1;
        _stored.srgb = 1;
      } else if (!memcmp(chunk.type, type_bkgd, 4)) {
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.bkgd) return PNG_EDUP_BKGD;

        if (_ihdr.color_type == 0 || _ihdr.color_type == 4) {
          if (chunk.length != 2) return PNG_ECHUNK_SIZE;

          _bkgd.gray = read_u16(data);
        } else if (_ihdr.color_type == 2 || _ihdr.color_type == 6) {
          if (chunk.length != 6) return PNG_ECHUNK_SIZE;

          _bkgd.red = read_u16(data);
          _bkgd.green = read_u16(data + 2);
          _bkgd.blue = read_u16(data + 4);
        } else if (_ihdr.color_type == 3) {
          if (chunk.length != 1) return PNG_ECHUNK_SIZE;
          if (!_file.plte) return PNG_EBKGD_NO_PLTE;

          _bkgd.plte_index = data[0];
          if (_bkgd.plte_index >= _plte.n_entries) return PNG_EBKGD_PLTE_IDX;
        }

        _file.bkgd = 1;
        _stored.bkgd = 1;
      } else if (!memcmp(chunk.type, type_trns, 4)) {
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.trns) return PNG_EDUP_TRNS;
        if (!chunk.length) return PNG_ECHUNK_SIZE;

        if (_ihdr.color_type == 0) {
          if (chunk.length != 2) return PNG_ECHUNK_SIZE;

          _trns.gray = read_u16(data);
        } else if (_ihdr.color_type == 2) {
          if (chunk.length != 6) return PNG_ECHUNK_SIZE;

          _trns.red = read_u16(data);
          _trns.green = read_u16(data + 2);
          _trns.blue = read_u16(data + 4);
        } else if (_ihdr.color_type == 3) {
          if (chunk.length > _plte.n_entries) return PNG_ECHUNK_SIZE;
          if (!_file.plte) return PNG_ETRNS_NO_PLTE;

          memcpy(_trns.type3_alpha, data, chunk.length);
          _trns.n_type3_entries = chunk.length;
        }

        if (_ihdr.color_type == 4 || _ihdr.color_type == 6) return PNG_ETRNS_COLOR_TYPE;

        _file.trns = 1;
        _stored.trns = 1;
      } else if (!memcmp(chunk.type, type_hist, 4)) {
        if (!_file.plte) return PNG_EHIST_NO_PLTE;
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.hist) return PNG_EDUP_HIST;

        if ((chunk.length / 2) != (_plte.n_entries)) return PNG_ECHUNK_SIZE;

        size_t k;
        for (k = 0; k < (chunk.length / 2); k++) {
          _hist.frequency[k] = read_u16(data + k * 2);
        }

        _file.hist = 1;
        _stored.hist = 1;
      } else if (!memcmp(chunk.type, type_phys, 4)) {
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.phys) return PNG_EDUP_PHYS;

        if (chunk.length != 9) return PNG_ECHUNK_SIZE;

        _phys.ppu_x = read_u32(data);
        _phys.ppu_y = read_u32(data + 4);
        _phys.unit_specifier = data[8];

        if (check_phys(_phys)) return PNG_EPHYS;

        _file.phys = 1;
        _stored.phys = 1;
      } else if (!memcmp(chunk.type, type_time, 4)) {
        if (_file.time) return PNG_EDUP_TIME;

        if (chunk.length != 7) return PNG_ECHUNK_SIZE;

        struct Time time;

        time.year = read_u16(data);
        time.month = data[2];
        time.day = data[3];
        time.hour = data[4];
        time.minute = data[5];
        time.second = data[6];

        if (check_time(time)) return PNG_ETIME;

        _file.time = 1;

        if (!_user.time) _time = time;

        _stored.time = 1;
      } else if (!memcmp(chunk.type, type_offs, 4)) {
        if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
        if (_file.offs) return PNG_EDUP_OFFS;

        if (chunk.length != 9) return PNG_ECHUNK_SIZE;

        _offs.x = read_s32(data);
        _offs.y = read_s32(data + 4);
        _offs.unit_specifier = data[8];

        if (check_offs(_offs)) return PNG_EOFFS;

        _file.offs = 1;
        _stored.offs = 1;
      } else /* Arbitrary-length chunk */
      {

        if (!memcmp(chunk.type, type_exif, 4)) {
          if (_file.exif) return PNG_EDUP_EXIF;

          _file.exif = 1;

          if (_user.exif) goto discard;

          if (increase_cache_usage(chunk.length, 1)) return PNG_ECHUNK_LIMITS;

          struct Exif exif;

          exif.length = chunk.length;

          exif.data = (char *) malloc(chunk.length);
          if (exif.data == nullptr) return PNG_EMEM;

          ret = read_chunk_bytes2(exif.data, chunk.length);
          if (ret) {
            free(exif.data);
            return ret;
          }

          if (check_exif(exif)) {
            free(exif.data);
            return PNG_EEXIF;
          }

          _exif = exif;

          _stored.exif = 1;
        } else if (!memcmp(chunk.type, type_iccp, 4)) {/* TODO: add test file with color profile */
          if (_file.plte) return PNG_ECHUNK_POS;
          if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
          if (_file.iccp) return PNG_EDUP_ICCP;
          if (!chunk.length) return PNG_ECHUNK_SIZE;

          _file.iccp = 1;

          uint32_t peek_bytes = 81 > chunk.length ? chunk.length : 81;

          ret = read_chunk_bytes(peek_bytes);
          if (ret) return ret;

          auto keyword_nul = (byte *) memchr(_data, '\0', peek_bytes);
          if (keyword_nul == nullptr) return PNG_EICCP_NAME;

          uint32_t keyword_len = keyword_nul - _data;

          if (keyword_len > 79) return PNG_EICCP_NAME;

          memcpy(_iccp.profile_name, _data, keyword_len);

          if (check_png_keyword(_iccp.profile_name)) return PNG_EICCP_NAME;

          if (chunk.length < (keyword_len + 2)) return PNG_ECHUNK_SIZE;

          if (_data[keyword_len + 1] != 0) return PNG_EICCP_COMPRESSION_METHOD;

          ret = inflate_stream(&_iccp.profile, &_iccp.profile_len, 0, _data + keyword_len + 2,
                               peek_bytes - (keyword_len + 2));

          if (ret) return ret;

          _stored.iccp = 1;
        } else if (!memcmp(chunk.type, type_text, 4) ||
                   !memcmp(chunk.type, type_ztxt, 4) ||
                   !memcmp(chunk.type, type_itxt, 4)) {
          if (!chunk.length) return PNG_ECHUNK_SIZE;

          _file.text = 1;

          if (_user.text) goto discard;

          if (increase_cache_usage(sizeof(struct Text2), 1)) return PNG_ECHUNK_LIMITS;

          _n_text++;
          if (_n_text < 1) return PNG_EOVERFLOW;
          if (sizeof(struct Text2) > SIZE_MAX / _n_text) return PNG_EOVERFLOW;

          void *buf = realloc(_text_list, _n_text * sizeof(struct Text2));
          if (buf == nullptr) return PNG_EMEM;
          _text_list = (struct Text2 *) buf;

          struct Text2 *text = &_text_list[_n_text - 1];
          memset(text, 0, sizeof(struct Text2));

          _undo = &PngParser::text_undo;

          uint32_t text_offset = 0, language_tag_offset = 0, translated_keyword_offset = 0;
          uint32_t peek_bytes = 256; /* enough for 3 80-byte keywords and some text bytes */
          uint32_t keyword_len;

          if (peek_bytes > chunk.length) peek_bytes = chunk.length;

          ret = read_chunk_bytes(peek_bytes);
          if (ret) return ret;

          data = _data;

          const byte *zlib_stream = nullptr;
          const unsigned char *peek_end = data + peek_bytes;
          auto keyword_nul = (const byte *) memchr(data, 0, chunk.length > 80 ? 80 : chunk.length);

          if (keyword_nul == nullptr) return PNG_ETEXT_KEYWORD;

          keyword_len = keyword_nul - data;

          if (!memcmp(chunk.type, type_text, 4)) {
            text->type = PNG_TEXT;

            text->text_length = chunk.length - keyword_len - 1;

            text_offset = keyword_len;

            /* increment past nul if there is a text field */
            if (text->text_length) text_offset++;
          } else if (!memcmp(chunk.type, type_ztxt, 4)) {
            text->type = PNG_ZTXT;

            if ((peek_bytes - keyword_len) <= 2) return PNG_EZTXT;

            if (keyword_nul[1]) return PNG_EZTXT_COMPRESSION_METHOD;

            text->compression_flag = 1;

            text_offset = keyword_len + 2;
          } else if (!memcmp(chunk.type, type_itxt, 4)) {
            text->type = PNG_ITXT;

            /* at least two 1-byte fields, two >=0 length strings, and one byte of (compressed) text */
            if ((peek_bytes - keyword_len) < 5) return PNG_EITXT;

            text->compression_flag = keyword_nul[1];

            if (text->compression_flag > 1) return PNG_EITXT_COMPRESSION_FLAG;

            if (keyword_nul[2]) return PNG_EITXT_COMPRESSION_METHOD;

            language_tag_offset = keyword_len + 3;

            auto term = (const byte *) memchr(data + language_tag_offset, 0, peek_bytes - language_tag_offset);
            if (term == nullptr) return PNG_EITXT_LANG_TAG;

            if ((peek_end - term) < 2) return PNG_EITXT;

            translated_keyword_offset = term - data + 1;

            zlib_stream = (const byte *) memchr(data + translated_keyword_offset, 0,
                                                peek_bytes - translated_keyword_offset);
            if (zlib_stream == nullptr) return PNG_EITXT;
            if (zlib_stream == peek_end) return PNG_EITXT;

            text_offset = zlib_stream - data + 1;
            text->text_length = chunk.length - text_offset;
          } else return PNG_EINTERNAL;


          if (text->compression_flag) {
            /* cache usage = peek_bytes + decompressed text size + nul */
            if (increase_cache_usage(peek_bytes, 0)) return PNG_ECHUNK_LIMITS;

            text->keyword = (char *) calloc(1, peek_bytes);
            if (text->keyword == nullptr) return PNG_EMEM;

            memcpy(text->keyword, data, peek_bytes);

            zlib_stream = _data + text_offset;

            ret = inflate_stream(&text->text, &text->text_length, 1, zlib_stream, peek_bytes - text_offset);

            if (ret) return ret;

            text->text[text->text_length - 1] = '\0';
            text->cache_usage = text->text_length + peek_bytes;
          } else {
            if (increase_cache_usage(chunk.length + 1, 0)) return PNG_ECHUNK_LIMITS;

            text->keyword = (char *) malloc(chunk.length + 1);
            if (text->keyword == nullptr) return PNG_EMEM;

            memcpy(text->keyword, data, peek_bytes);

            if (chunk.length > peek_bytes) {
              ret = read_chunk_bytes2(text->keyword + peek_bytes, chunk.length - peek_bytes);
              if (ret) return ret;
            }

            text->text = text->keyword + text_offset;

            text->text_length = chunk.length - text_offset;

            text->text[text->text_length] = '\0';
            text->cache_usage = chunk.length + 1;
          }

          if (check_png_keyword(text->keyword)) return PNG_ETEXT_KEYWORD;

          text->text_length = strlen(text->text);

          if (text->type != PNG_ITXT) {
            language_tag_offset = keyword_len;
            translated_keyword_offset = keyword_len;

            if (_strict && check_png_text(text->text, text->text_length)) {
              if (text->type == PNG_ZTXT) return PNG_EZTXT;
              else return PNG_ETEXT;
            }
          }

          text->language_tag = text->keyword + language_tag_offset;
          text->translated_keyword = text->keyword + translated_keyword_offset;

          _stored.text = 1;
        } else if (!memcmp(chunk.type, type_splt, 4)) {
          if (_state == PNG_STATE_AFTER_IDAT) return PNG_ECHUNK_POS;
          if (_user.splt) goto discard; /* XXX: could check profile names for uniqueness */
          if (!chunk.length) return PNG_ECHUNK_SIZE;

          _file.splt = 1;

          /* chunk.length + sizeof(struct SPLT) + splt->n_entries * sizeof(struct SPLTEntry) */
          if (increase_cache_usage(chunk.length + sizeof(struct SPLT), 1)) return PNG_ECHUNK_LIMITS;

          _n_splt++;
          if (_n_splt < 1) return PNG_EOVERFLOW;
          if (sizeof(struct SPLT) > SIZE_MAX / _n_splt) return PNG_EOVERFLOW;

          void *buf = realloc(_splt_list, _n_splt * sizeof(struct SPLT));
          if (buf == nullptr) return PNG_EMEM;
          _splt_list = (struct SPLT *) buf;

          struct SPLT *splt = &_splt_list[_n_splt - 1];

          memset(splt, 0, sizeof(struct SPLT));

          _undo = &PngParser::splt_undo;

          void *t = malloc(chunk.length);
          if (t == nullptr) return PNG_EMEM;

          splt->entries = (struct SPLTEntry *) t; /* simplifies error handling */
          data = (byte *) t;

          ret = read_chunk_bytes2(t, chunk.length);
          if (ret) return ret;

          uint32_t keyword_len = chunk.length < 80 ? chunk.length : 80;

          auto keyword_nul = (const byte *) memchr(data, 0, keyword_len);
          if (keyword_nul == nullptr) return PNG_ESPLT_NAME;

          keyword_len = keyword_nul - data;

          memcpy(splt->name, data, keyword_len);

          if (check_png_keyword(splt->name)) return PNG_ESPLT_NAME;

          uint32_t j;
          for (j = 0; j < (_n_splt - 1); j++) {
            if (!strcmp(_splt_list[j].name, splt->name)) return PNG_ESPLT_DUP_NAME;
          }

          if ((chunk.length - keyword_len) <= 2) return PNG_ECHUNK_SIZE;

          splt->sample_depth = data[keyword_len + 1];

          uint32_t entries_len = chunk.length - keyword_len - 2;
          if (!entries_len) return PNG_ECHUNK_SIZE;

          if (splt->sample_depth == 16) {
            if (entries_len % 10 != 0) return PNG_ECHUNK_SIZE;
            splt->n_entries = entries_len / 10;
          } else if (splt->sample_depth == 8) {
            if (entries_len % 6 != 0) return PNG_ECHUNK_SIZE;
            splt->n_entries = entries_len / 6;
          } else return PNG_ESPLT_DEPTH;

          if (!splt->n_entries) return PNG_ECHUNK_SIZE;

          size_t list_size = splt->n_entries;

          if (list_size > SIZE_MAX / sizeof(struct SPLTEntry)) return PNG_EOVERFLOW;

          list_size *= sizeof(struct SPLTEntry);

          if (increase_cache_usage(list_size, 0)) return PNG_ECHUNK_LIMITS;

          splt->entries = (struct SPLTEntry *) malloc(list_size);
          if (splt->entries == nullptr) {
            free(t);
            return PNG_EMEM;
          }

          data = (unsigned char *) t + keyword_len + 2;

          uint32_t k;
          if (splt->sample_depth == 16) {
            for (k = 0; k < splt->n_entries; k++) {
              splt->entries[k].red = read_u16(data + k * 10);
              splt->entries[k].green = read_u16(data + k * 10 + 2);
              splt->entries[k].blue = read_u16(data + k * 10 + 4);
              splt->entries[k].alpha = read_u16(data + k * 10 + 6);
              splt->entries[k].frequency = read_u16(data + k * 10 + 8);
            }
          } else if (splt->sample_depth == 8) {
            for (k = 0; k < splt->n_entries; k++) {
              splt->entries[k].red = data[k * 6];
              splt->entries[k].green = data[k * 6 + 1];
              splt->entries[k].blue = data[k * 6 + 2];
              splt->entries[k].alpha = data[k * 6 + 3];
              splt->entries[k].frequency = read_u16(data + k * 6 + 4);
            }
          }

          free(t);
          decrease_cache_usage(chunk.length);

          _stored.splt = 1;
        } else /* Unknown chunk */
        {
          _file.unknown = 1;

          if (!_keep_unknown) goto discard;
          if (_user.unknown) goto discard;

          if (increase_cache_usage(chunk.length + sizeof(struct UnknownChunk), 1))
            return PNG_ECHUNK_LIMITS;

          _n_chunks++;
          if (_n_chunks < 1) return PNG_EOVERFLOW;
          if (sizeof(struct UnknownChunk) > SIZE_MAX / _n_chunks) return PNG_EOVERFLOW;

          void *buf = realloc(_chunk_list, _n_chunks * sizeof(struct UnknownChunk));
          if (buf == nullptr) return PNG_EMEM;
          _chunk_list = (struct UnknownChunk *) buf;

          struct UnknownChunk *chunkp = &_chunk_list[_n_chunks - 1];

          memset(chunkp, 0, sizeof(struct UnknownChunk));

          _undo = &PngParser::chunk_undo;

          memcpy(chunkp->type, chunk.type, 4);

          if (_state < PNG_STATE_FIRST_IDAT) {
            if (_file.plte) chunkp->location = PNG_AFTER_PLTE;
            else chunkp->location = PNG_AFTER_IHDR;
          } else if (_state >= PNG_STATE_AFTER_IDAT) chunkp->location = PNG_AFTER_IDAT;

          if (chunk.length > 0) {
            void *t = malloc(chunk.length);
            if (t == nullptr) return PNG_EMEM;

            ret = read_chunk_bytes2(t, chunk.length);
            if (ret) {
              free(t);
              return ret;
            }

            chunkp->length = chunk.length;
            chunkp->data = t;
          }

          _stored.unknown = 1;
        }

        discard:
        ret = discard_chunk_bytes(_cur_chunk_bytes_left);
        if (ret) return ret;
      }

    }

    return ret;
  }

/* Read chunks before or after the IDAT chunks depending on state */

  void PngParser::read_chunks() {
    if (_data == nullptr && !_encode_only) {
      throw upan::exception(XLOC, _strerror(PNG_EINTERNAL));
    }

    if (_state == PNG_STATE_INPUT) {
      read_ihdr();
      _state = PNG_STATE_IHDR;
    }

    if (_state == PNG_STATE_EOI) {
      _state = PNG_STATE_AFTER_IDAT;
      _prev_was_idat = 1;
    }

    while (_state < PNG_STATE_FIRST_IDAT || _state == PNG_STATE_AFTER_IDAT) {
      int ret = read_non_idat_chunks();

      if (!ret) {
        if (_state < PNG_STATE_FIRST_IDAT) _state = PNG_STATE_FIRST_IDAT;
        else if (_state == PNG_STATE_AFTER_IDAT) _state = PNG_STATE_IEND;
      } else {
        switch (ret) {
          case PNG_ECHUNK_POS:
          case PNG_ECHUNK_SIZE: /* size != expected size, PNG_ECHUNK_STDLEN = invalid size */
          case PNG_EDUP_PLTE:
          case PNG_EDUP_CHRM:
          case PNG_EDUP_GAMA:
          case PNG_EDUP_ICCP:
          case PNG_EDUP_SBIT:
          case PNG_EDUP_SRGB:
          case PNG_EDUP_BKGD:
          case PNG_EDUP_HIST:
          case PNG_EDUP_TRNS:
          case PNG_EDUP_PHYS:
          case PNG_EDUP_TIME:
          case PNG_EDUP_OFFS:
          case PNG_EDUP_EXIF:
          case PNG_ECHRM:
          case PNG_ETRNS_COLOR_TYPE:
          case PNG_ETRNS_NO_PLTE:
          case PNG_EGAMA:
          case PNG_EICCP_NAME:
          case PNG_EICCP_COMPRESSION_METHOD:
          case PNG_ESBIT:
          case PNG_ESRGB:
          case PNG_ETEXT:
          case PNG_ETEXT_KEYWORD:
          case PNG_EZTXT:
          case PNG_EZTXT_COMPRESSION_METHOD:
          case PNG_EITXT:
          case PNG_EITXT_COMPRESSION_FLAG:
          case PNG_EITXT_COMPRESSION_METHOD:
          case PNG_EITXT_LANG_TAG:
          case PNG_EITXT_TRANSLATED_KEY:
          case PNG_EBKGD_NO_PLTE:
          case PNG_EBKGD_PLTE_IDX:
          case PNG_EHIST_NO_PLTE:
          case PNG_EPHYS:
          case PNG_ESPLT_NAME:
          case PNG_ESPLT_DUP_NAME:
          case PNG_ESPLT_DEPTH:
          case PNG_ETIME:
          case PNG_EOFFS:
          case PNG_EEXIF:
          case PNG_EZLIB: {
            if (!_strict && !_current_chunk.is_critical_chunk()) {
              ret = discard_chunk_bytes(_cur_chunk_bytes_left);
              if (ret) {
                throw upan::exception(XLOC, _strerror(ret));
              }

              if (_undo) (this->*_undo)();

              _stored = _prev_stored;

              _discard = 0;
              _undo = nullptr;

              continue;
            } else {
              throw upan::exception(XLOC, _strerror(ret));
            }

            break;
          }
          default:
            throw upan::exception(XLOC, _strerror(ret));
        }
      }
    }
  }

  int PngParser::read_scanline() {
    int ret, pass = _row_info.pass;
    struct RowInfo *ri = &_row_info;
    const struct SubImage *sub = _subimage;
    size_t scanline_width = sub[pass].scanline_width;
    uint32_t scanline_idx = ri->scanline_idx;

    uint8_t next_filter = 0;

    if (scanline_idx == (sub[pass].height - 1) && ri->pass == _last_pass) {
      ret = read_scanline_bytes(_scanline, scanline_width - 1);
    } else {
      ret = read_scanline_bytes(_scanline, scanline_width);
      if (ret) return ret;

      next_filter = _scanline[scanline_width - 1];
      if (next_filter > 4) ret = PNG_EFILTER;
    }

    if (ret) return ret;

    if (!scanline_idx && ri->filter > 1) {
      /* prev_scanline is all zeros for the first scanline */
      memset(_prev_scanline, 0, scanline_width);
    }

    if (_ihdr.bit_depth == 16 && _fmt != PNG_FMT_RAW) u16_row_to_host(_scanline, scanline_width - 1);

    ret = defilter_scanline(_prev_scanline, _scanline, scanline_width, _bytes_per_pixel, ri->filter);
    if (ret) return ret;

    ri->filter = next_filter;

    return 0;
  }

  int PngParser::update_row_info() {
    int interlacing = _ihdr.interlace_method;
    struct RowInfo *ri = &_row_info;
    const struct SubImage *sub = _subimage;

    if (ri->scanline_idx == (sub[ri->pass].height - 1)) /* Last scanline */
    {
      if (ri->pass == _last_pass) {
        _state = PNG_STATE_EOI;

        return PNG_EOI;
      }

      ri->scanline_idx = 0;
      ri->pass++;

      /* Skip empty passes */
      while ((!sub[ri->pass].width || !sub[ri->pass].height) && (ri->pass < _last_pass)) ri->pass++;
    } else {
      ri->row_num++;
      ri->scanline_idx++;
    }

    if (interlacing) ri->row_num = adam7_y_start[ri->pass] + ri->scanline_idx * adam7_y_delta[ri->pass];

    return 0;
  }

  int PngParser::decode_scanline(void *out, size_t len) {
    if (out == nullptr) return 1;

    if (_state >= PNG_STATE_EOI) return PNG_EOI;

    struct DecodeFlagStat f = _decode_flags;

    struct RowInfo *ri = &_row_info;
    const struct SubImage *sub = _subimage;

    const iHeader *ihdr = &_ihdr;
    const uint16_t *gamma_lut = _gamma_lut;
    unsigned char *trns_px = _trns_px;
    const struct SBIT *sb = &_decode_sb;
    const struct PLTEEntry *plte = _decode_plte.rgba;
    _iter iter(ihdr->bit_depth, _scanline);

    const unsigned char *scanline;

    const int pass = ri->pass;
    const int fmt = _fmt;
    const size_t scanline_width = sub[pass].scanline_width;
    const uint32_t width = sub[pass].width;
    uint32_t k;
    uint8_t r_8, g_8, b_8, a_8, gray_8;
    uint16_t r_16, g_16, b_16, a_16, gray_16;
    r_8 = 0;
    g_8 = 0;
    b_8 = 0;
    a_8 = 0;
    gray_8 = 0;
    r_16 = 0;
    g_16 = 0;
    b_16 = 0;
    a_16 = 0;
    gray_16 = 0;
    size_t pixel_size = 4; /* PNG_FMT_RGBA8 */
    size_t pixel_offset = 0;
    unsigned char *pixel;
    unsigned processing_depth = ihdr->bit_depth;

    if (f.indexed) processing_depth = 8;

    if (fmt == PNG_FMT_RGBA16) pixel_size = 8;
    else if (fmt == PNG_FMT_RGB8) pixel_size = 3;

    if (len < sub[pass].out_width) return PNG_EBUFSIZ;

    int ret = read_scanline();

    if (ret) return on_error(ret);

    scanline = _scanline;

    for (k = 0; k < width; k++) {
      pixel = (unsigned char *) out + pixel_offset;
      pixel_offset += pixel_size;

      if (f.same_layout) {
        if (f.zerocopy) break;

        memcpy(out, scanline, scanline_width - 1);
        break;
      }

      if (f.unpack) {
        unpack_scanline((byte *) out, scanline, width, ihdr->bit_depth, fmt);
        break;
      }

      if (ihdr->color_type == PNG_COLOR_TYPE_TRUECOLOR) {
        if (ihdr->bit_depth == 16) {
          memcpy(&r_16, scanline + (k * 6), 2);
          memcpy(&g_16, scanline + (k * 6) + 2, 2);
          memcpy(&b_16, scanline + (k * 6) + 4, 2);

          a_16 = 65535;
        } else /* == 8 */
        {
          if (fmt == PNG_FMT_RGBA8) {
            rgb8_row_to_rgba8(scanline, (byte *) out, width);
            break;
          }

          r_8 = scanline[k * 3];
          g_8 = scanline[k * 3 + 1];
          b_8 = scanline[k * 3 + 2];

          a_8 = 255;
        }
      } else if (ihdr->color_type == PNG_COLOR_TYPE_INDEXED) {
        uint8_t entry = 0;

        if (ihdr->bit_depth == 8) {
          if (fmt & (PNG_FMT_RGBA8 | PNG_FMT_RGB8)) {
            expand_row((byte *) out, scanline, &_decode_plte, width, fmt);
            break;
          }

          entry = scanline[k];
        } else /* < 8 */
        {
          entry = iter.get_sample();
        }

        if (fmt & (PNG_FMT_RGBA8 | PNG_FMT_RGB8)) {
          pixel[0] = plte[entry].red;
          pixel[1] = plte[entry].green;
          pixel[2] = plte[entry].blue;
          if (fmt == PNG_FMT_RGBA8) pixel[3] = plte[entry].alpha;

          continue;
        } else /* RGBA16 */
        {
          r_16 = plte[entry].red;
          g_16 = plte[entry].green;
          b_16 = plte[entry].blue;
          a_16 = plte[entry].alpha;

          r_16 = (r_16 << 8) | r_16;
          g_16 = (g_16 << 8) | g_16;
          b_16 = (b_16 << 8) | b_16;
          a_16 = (a_16 << 8) | a_16;

          memcpy(pixel, &r_16, 2);
          memcpy(pixel + 2, &g_16, 2);
          memcpy(pixel + 4, &b_16, 2);
          memcpy(pixel + 6, &a_16, 2);

          continue;
        }
      } else if (ihdr->color_type == PNG_COLOR_TYPE_TRUECOLOR_ALPHA) {
        if (ihdr->bit_depth == 16) {
          memcpy(&r_16, scanline + (k * 8), 2);
          memcpy(&g_16, scanline + (k * 8) + 2, 2);
          memcpy(&b_16, scanline + (k * 8) + 4, 2);
          memcpy(&a_16, scanline + (k * 8) + 6, 2);
        } else /* == 8 */
        {
          r_8 = scanline[k * 4];
          g_8 = scanline[k * 4 + 1];
          b_8 = scanline[k * 4 + 2];
          a_8 = scanline[k * 4 + 3];
        }
      } else if (ihdr->color_type == PNG_COLOR_TYPE_GRAYSCALE) {
        if (ihdr->bit_depth == 16) {
          memcpy(&gray_16, scanline + k * 2, 2);

          if (f.apply_trns && _trns.gray == gray_16) a_16 = 0;
          else a_16 = 65535;

          r_16 = gray_16;
          g_16 = gray_16;
          b_16 = gray_16;
        } else /* <= 8 */
        {
          gray_8 = iter.get_sample();

          if (f.apply_trns && _trns.gray == gray_8) a_8 = 0;
          else a_8 = 255;

          r_8 = gray_8;
          g_8 = gray_8;
          b_8 = gray_8;
        }
      } else if (ihdr->color_type == PNG_COLOR_TYPE_GRAYSCALE_ALPHA) {
        if (ihdr->bit_depth == 16) {
          memcpy(&gray_16, scanline + (k * 4), 2);
          memcpy(&a_16, scanline + (k * 4) + 2, 2);

          r_16 = gray_16;
          g_16 = gray_16;
          b_16 = gray_16;
        } else /* == 8 */
        {
          gray_8 = scanline[k * 2];
          a_8 = scanline[k * 2 + 1];

          r_8 = gray_8;
          g_8 = gray_8;
          b_8 = gray_8;
        }
      }


      if (fmt & (PNG_FMT_RGBA8 | PNG_FMT_RGB8)) {
        if (ihdr->bit_depth == 16) {
          r_8 = r_16 >> 8;
          g_8 = g_16 >> 8;
          b_8 = b_16 >> 8;
          a_8 = a_16 >> 8;
        }

        pixel[0] = r_8;
        pixel[1] = g_8;
        pixel[2] = b_8;

        if (fmt == PNG_FMT_RGBA8) pixel[3] = a_8;
      } else if (fmt == PNG_FMT_RGBA16) {
        if (ihdr->bit_depth != 16) {
          r_16 = r_8;
          g_16 = g_8;
          b_16 = b_8;
          a_16 = a_8;
        }

        memcpy(pixel, &r_16, 2);
        memcpy(pixel + 2, &g_16, 2);
        memcpy(pixel + 4, &b_16, 2);
        memcpy(pixel + 6, &a_16, 2);
      }
    }/* for(k=0; k < width; k++) */

    if (f.apply_trns) trns_row((byte *) out, scanline, trns_px, _bytes_per_pixel, &_ihdr, width, fmt);

    if (f.do_scaling) scale_row((byte *) out, width, fmt, processing_depth, sb);

    if (f.apply_gamma) gamma_correct_row((byte *) out, width, fmt, gamma_lut);

    /* The previous scanline is always defiltered */
    void *t = _prev_scanline;
    _prev_scanline = _scanline;
    _scanline = (byte *) t;

    ret = update_row_info();

    if (ret == PNG_EOI) {
      if (_cur_chunk_bytes_left) /* zlib stream ended before an IDAT chunk boundary */
      {/* Discard the rest of the chunk */
        int error = discard_chunk_bytes(_cur_chunk_bytes_left);
        if (error) return on_error(error);
      }

      _last_idat = _current_chunk;
    }

    return ret;
  }

  int PngParser::decode_row(void *out, size_t len) {
    if (out == nullptr) return 1;
    if (_state >= PNG_STATE_EOI) return PNG_EOI;
    if (len < _image_width) return PNG_EBUFSIZ;

    const iHeader *ihdr = &_ihdr;
    int ret, pass = _row_info.pass;
    auto outptr = (byte *) out;

    if (!ihdr->interlace_method || pass == 6) return decode_scanline(out, len);

    ret = decode_scanline(_row, _image_width);
    if (ret && ret != PNG_EOI) return ret;

    uint32_t k;
    unsigned pixel_size = 4; /* RGBA8 */
    if (_fmt == PNG_FMT_RGBA16) pixel_size = 8;
    else if (_fmt == PNG_FMT_RGB8) pixel_size = 3;
    else if (_fmt & (PNG_FMT_PNG | PNG_FMT_RAW)) {
      if (ihdr->bit_depth < 8) {
        _iter iter(ihdr->bit_depth, _row);
        const uint8_t samples_per_byte = 8 / ihdr->bit_depth;
        uint8_t sample;

        for (k = 0; k < _subimage[pass].width; k++) {
          sample = iter.get_sample();

          size_t ioffset = adam7_x_start[pass] + k * adam7_x_delta[pass];

          sample = sample << (iter.initial_shift - ioffset * ihdr->bit_depth % 8);

          ioffset /= samples_per_byte;

          outptr[ioffset] |= sample;
        }

        return 0;
      } else pixel_size = _bytes_per_pixel;
    }

    for (k = 0; k < _subimage[pass].width; k++) {
      size_t ioffset = (adam7_x_start[pass] + (size_t) k * adam7_x_delta[pass]) * pixel_size;

      memcpy(outptr + ioffset, _row + k * pixel_size, pixel_size);
    }

    return 0;
  }

  int PngParser::decode_image(void *out, size_t len, Format fmt, int flags) {
    if (_encode_only) return PNG_ECTXTYPE;
    if (_state >= PNG_STATE_EOI) return PNG_EOI;

    const iHeader& ihdr = _ihdr;

    int ret = check_decode_fmt(fmt);
    if (ret) return ret;

    ret = ihdr.calculate_image_width(fmt, _image_width);
    if (ret) return on_error(ret);

    if (_image_width > SIZE_MAX / ihdr.height) _image_size = 0; /* overflow */
    else _image_size = _image_width * ihdr.height;

    if (!(flags & PNG_DECODE_PROGRESSIVE)) {
      if (out == nullptr) return 1;
      if (!_image_size) return PNG_EOVERFLOW;
      if (len < _image_size) return PNG_EBUFSIZ;
    }

    uint32_t bytes_read = 0;

    ret = read_idat_bytes(&bytes_read);
    if (ret) return on_error(ret);

    if (bytes_read > 1) {
      int valid = read_u16(_data) % 31 ? 0 : 1;

      unsigned flg = _data[1];
      unsigned flevel = flg >> 6;
      int compression_level = Z_DEFAULT_COMPRESSION;

      if (flevel == 0) compression_level = Z_NO_COMPRESSION; /* fastest */
      else if (flevel == 1) compression_level = Z_BEST_SPEED; /* fast */
      else if (flevel == 2) compression_level = Z_DEFAULT_COMPRESSION; /* default */
      else if (flevel == 3) compression_level = Z_BEST_COMPRESSION; /* slowest, max compression */

      if (valid) _image_options._compressionLevel = compression_level;
    }

    ret = inflate_init(_image_options._windowBits);
    if (ret) return on_error(ret);

    _zstream.avail_in = bytes_read;
    _zstream.next_in = _data;

    size_t scanline_buf_size = _subimage[_widest_pass].scanline_width;

    scanline_buf_size += 32;

    if (scanline_buf_size < 32) return PNG_EOVERFLOW;

    _scanline_buf = (byte *) malloc(scanline_buf_size);
    _prev_scanline_buf = (byte *) malloc(scanline_buf_size);

    _scanline = _scanline_buf;
    _prev_scanline = _prev_scanline_buf;

    struct DecodeFlagStat f = {0};

    _fmt = fmt;

    if (ihdr.color_type == PNG_COLOR_TYPE_INDEXED) f.indexed = 1;

    unsigned processing_depth = ihdr.bit_depth;

    if (f.indexed) processing_depth = 8;

    if (ihdr.interlace_method) {
      f.interlaced = 1;
      _row_buf = (byte *) malloc(_image_width);
      _row = _row_buf;

      if (_row == nullptr) return on_error(PNG_EMEM);
    }

    if (_scanline == nullptr || _prev_scanline == nullptr) {
      return on_error(PNG_EMEM);
    }

    f.do_scaling = 1;
    if (f.indexed) f.do_scaling = 0;

    unsigned depth_target = 8; /* FMT_RGBA8, G8 */
    if (fmt == PNG_FMT_RGBA16) depth_target = 16;

    if (flags & PNG_DECODE_TRNS && _stored.trns) f.apply_trns = 1;
    else flags &= ~PNG_DECODE_TRNS;

    if (ihdr.color_type == PNG_COLOR_TYPE_GRAYSCALE_ALPHA ||
        ihdr.color_type == PNG_COLOR_TYPE_TRUECOLOR_ALPHA)
      flags &= ~PNG_DECODE_TRNS;

    if (flags & PNG_DECODE_GAMMA && _stored.gama) f.apply_gamma = 1;
    else flags &= ~PNG_DECODE_GAMMA;

    if (flags & PNG_DECODE_USE_SBIT && _stored.sbit) f.use_sbit = 1;
    else flags &= ~PNG_DECODE_USE_SBIT;

    if (fmt & (PNG_FMT_RGBA8 | PNG_FMT_RGBA16)) {
      if (ihdr.color_type == PNG_COLOR_TYPE_TRUECOLOR_ALPHA &&
          ihdr.bit_depth == depth_target)
        f.same_layout = 1;
    } else if (fmt == PNG_FMT_RGB8) {
      if (ihdr.color_type == PNG_COLOR_TYPE_TRUECOLOR &&
          ihdr.bit_depth == depth_target)
        f.same_layout = 1;

      f.apply_trns = 0; /* not applicable */
    } else if (fmt & (PNG_FMT_PNG | PNG_FMT_RAW)) {
      f.same_layout = 1;
      f.do_scaling = 0;
      f.apply_gamma = 0; /* for now */
      f.apply_trns = 0;
    }

    /*if(f.same_layout && !flags && !f.interlaced) f.zerocopy = 1;*/

    uint16_t *gamma_lut = nullptr;

    if (f.apply_gamma) {
      float file_gamma = (float) _gama / 100000.0f;
      float max;

      unsigned lut_entries;

      if (fmt & (PNG_FMT_RGBA8 | PNG_FMT_RGB8)) {
        lut_entries = 256;
        max = 255.0f;

        gamma_lut = _gamma_lut8;
        _gamma_lut = _gamma_lut8;
      } else /* PNG_FMT_RGBA16 */
      {
        lut_entries = 65536;
        max = 65535.0f;

        _gamma_lut16 = (uint16_t *) malloc(lut_entries * sizeof(uint16_t));
        if (_gamma_lut16 == nullptr) return on_error(PNG_EMEM);

        gamma_lut = _gamma_lut16;
        _gamma_lut = _gamma_lut16;
      }

      float screen_gamma = 2.2f;
      float exponent = file_gamma * screen_gamma;

      if (FP_ZERO == fpclassify(exponent)) return on_error(PNG_EGAMA);

      exponent = 1.0f / exponent;

      unsigned i;
      for (i = 0; i < lut_entries; i++) {
        float c = pow((float) i / max, exponent) * max;
        if (c > max) c = max;

        gamma_lut[i] = (uint16_t) c;
      }
    }

    struct SBIT *sb = &_decode_sb;

    sb->red_bits = processing_depth;
    sb->green_bits = processing_depth;
    sb->blue_bits = processing_depth;
    sb->alpha_bits = processing_depth;
    sb->grayscale_bits = processing_depth;

    if (f.use_sbit) {
      if (ihdr.color_type == 0) {
        sb->grayscale_bits = _sbit.grayscale_bits;
        sb->alpha_bits = ihdr.bit_depth;
      } else if (ihdr.color_type == 2 || ihdr.color_type == 3) {
        sb->red_bits = _sbit.red_bits;
        sb->green_bits = _sbit.green_bits;
        sb->blue_bits = _sbit.blue_bits;
        sb->alpha_bits = ihdr.bit_depth;
      } else if (ihdr.color_type == 4) {
        sb->grayscale_bits = _sbit.grayscale_bits;
        sb->alpha_bits = _sbit.alpha_bits;
      } else /* == 6 */
      {
        sb->red_bits = _sbit.red_bits;
        sb->green_bits = _sbit.green_bits;
        sb->blue_bits = _sbit.blue_bits;
        sb->alpha_bits = _sbit.alpha_bits;
      }
    }

    if (ihdr.bit_depth == 16 &&
        fmt & (PNG_FMT_RGBA8 | PNG_FMT_RGB8)) {/* samples are scaled down by 8 bits in the decode loop */
      sb->red_bits -= 8;
      sb->green_bits -= 8;
      sb->blue_bits -= 8;
      sb->alpha_bits -= 8;
      sb->grayscale_bits -= 8;

      processing_depth = 8;
    }

    /* Prevent infinite loops in sample_to_target() */
    if (!depth_target || depth_target > 16 ||
        !processing_depth || processing_depth > 16 ||
        !sb->grayscale_bits || sb->grayscale_bits > processing_depth ||
        !sb->alpha_bits || sb->alpha_bits > processing_depth ||
        !sb->red_bits || sb->red_bits > processing_depth ||
        !sb->green_bits || sb->green_bits > processing_depth ||
        !sb->blue_bits || sb->blue_bits > processing_depth) {
      return on_error(PNG_ESBIT);
    }

    if (sb->red_bits == sb->green_bits &&
        sb->green_bits == sb->blue_bits &&
        sb->blue_bits == sb->alpha_bits &&
        sb->alpha_bits == processing_depth &&
        processing_depth == depth_target)
      f.do_scaling = 0;

    struct PLTEEntry *plte = _decode_plte.rgba;

    /* Pre-process palette entries */
    if (f.indexed) {
      uint8_t red, green, blue, alpha;

      uint32_t i;
      for (i = 0; i < 256; i++) {
        if (f.apply_trns && i < _trns.n_type3_entries)
          _plte.entries[i].alpha = _trns.type3_alpha[i];
        else
          _plte.entries[i].alpha = 255;

        red = sample_to_target(_plte.entries[i].red, 8, sb->red_bits, 8);
        green = sample_to_target(_plte.entries[i].green, 8, sb->green_bits, 8);
        blue = sample_to_target(_plte.entries[i].blue, 8, sb->blue_bits, 8);
        alpha = sample_to_target(_plte.entries[i].alpha, 8, sb->alpha_bits, 8);

        plte[i].red = red;
        plte[i].green = green;
        plte[i].blue = blue;
        plte[i].alpha = alpha;
      }

      f.apply_trns = 0;
    }

    unsigned char *trns_px = _trns_px;

    if (f.apply_trns) {
      uint16_t mask = ~0;
      if (_ihdr.bit_depth < 16) mask = (1 << _ihdr.bit_depth) - 1;

      if (fmt & (PNG_FMT_RGBA8 | PNG_FMT_RGBA16)) {
        if (ihdr.color_type == PNG_COLOR_TYPE_TRUECOLOR) {
          if (ihdr.bit_depth == 16) {
            memcpy(trns_px, &_trns.red, 2);
            memcpy(trns_px + 2, &_trns.green, 2);
            memcpy(trns_px + 4, &_trns.blue, 2);
          } else {
            trns_px[0] = _trns.red & mask;
            trns_px[1] = _trns.green & mask;
            trns_px[2] = _trns.blue & mask;
          }
        }
      } else if (ihdr.color_type == PNG_COLOR_TYPE_GRAYSCALE) // fmt == PNG_FMT_GA8 &&
      {
        if (ihdr.bit_depth == 16) {
          memcpy(trns_px, &_trns.gray, 2);
        } else {
          trns_px[0] = _trns.gray & mask;
        }
      }
    }

    _decode_flags = f;

    _state = PNG_STATE_DECODE_INIT;

    struct RowInfo *ri = &_row_info;
    struct SubImage *sub = _subimage;

    while (!sub[ri->pass].width || !sub[ri->pass].height) ri->pass++;

    if (f.interlaced) ri->row_num = adam7_y_start[ri->pass];

    unsigned pixel_size = 4; /* PNG_FMT_RGBA8 */

    if (fmt == PNG_FMT_RGBA16) pixel_size = 8;
    else if (fmt == PNG_FMT_RGB8) pixel_size = 3;

    int i;
    for (i = ri->pass; i <= _last_pass; i++) {
      if (!sub[i].scanline_width) continue;

      if (fmt & (PNG_FMT_PNG | PNG_FMT_RAW)) sub[i].out_width = sub[i].scanline_width - 1;
      else sub[i].out_width = (size_t) sub[i].width * pixel_size;

      if (sub[i].out_width > UINT32_MAX) return on_error(PNG_EOVERFLOW);
    }

    /* Read the first filter byte, offsetting all reads by 1 byte.
    The scanlines will be aligned with the start of the array with
    the next scanline's filter byte at the end,
    the last scanline will end up being 1 byte "shorter". */
    ret = read_scanline_bytes(&ri->filter, 1);
    if (ret) return on_error(ret);

    if (ri->filter > 4) return on_error(PNG_EFILTER);

    if (flags & PNG_DECODE_PROGRESSIVE) {
      return 0;
    }

    do {
      size_t ioffset = ri->row_num * _image_width;

      ret = decode_row((unsigned char *) out + ioffset, _image_width);
    } while (!ret);

    if (ret != PNG_EOI) return on_error(ret);

    return 0;
  }

  int PngParser::get_row_info(struct RowInfo *row_info) {
    if (row_info == nullptr || _state < PNG_STATE_DECODE_INIT) return 1;

    if (_state >= PNG_STATE_EOI) return PNG_EOI;

    *row_info = _row_info;

    return 0;
  }

  int PngParser::write_chunks_before_idat() {
    if (!_encode_only) return PNG_EINTERNAL;
    if (!_stored.ihdr) return PNG_EINTERNAL;

    int ret;
    uint32_t i;
    size_t length;
    const iHeader *ihdr = &_ihdr;
    unsigned char *data = _decode_plte.raw;

    ret = write_data(png_signature, 8);
    if (ret) return ret;

    write_u32(data, ihdr->width);
    write_u32(data + 4, ihdr->height);
    data[8] = ihdr->bit_depth;
    data[9] = ihdr->color_type;
    data[10] = ihdr->compression_method;
    data[11] = ihdr->filter_method;
    data[12] = ihdr->interlace_method;

    ret = write_chunk(type_ihdr, data, 13);
    if (ret) return ret;

    if (_stored.chrm) {
      write_u32(data, _chrm_int.white_point_x);
      write_u32(data + 4, _chrm_int.white_point_y);
      write_u32(data + 8, _chrm_int.red_x);
      write_u32(data + 12, _chrm_int.red_y);
      write_u32(data + 16, _chrm_int.green_x);
      write_u32(data + 20, _chrm_int.green_y);
      write_u32(data + 24, _chrm_int.blue_x);
      write_u32(data + 28, _chrm_int.blue_y);

      ret = write_chunk(type_chrm, data, 32);
      if (ret) return ret;
    }

    if (_stored.gama) {
      write_u32(data, _gama);

      ret = write_chunk(type_gama, data, 4);
      if (ret) return ret;
    }

    if (_stored.iccp) {
      unsigned long dest_len = gccucore::zlib::compressBound((unsigned long) _iccp.profile_len);

      auto buf = (byte *) malloc(dest_len);
      if (buf == nullptr) return PNG_EMEM;

      ret = gccucore::zlib::compress2(buf, &dest_len, (byte *) _iccp.profile, (unsigned long) _iccp.profile_len,
                                      Z_DEFAULT_COMPRESSION);

      if (ret != Z_OK) {
        free(buf);
        return PNG_EZLIB;
      }

      size_t name_len = strlen(_iccp.profile_name);

      length = name_len + 2;
      length += dest_len;

      if (dest_len > length) return PNG_EOVERFLOW;

      unsigned char *cdata = nullptr;

      ret = write_header(type_iccp, length, &cdata);

      if (ret) {
        free(buf);
        return ret;
      }

      memcpy(cdata, _iccp.profile_name, name_len + 1);
      cdata[name_len + 1] = 0; /* compression method */
      memcpy(cdata + name_len + 2, buf, dest_len);

      free(buf);

      ret = finish_chunk();
      if (ret) return ret;
    }

    if (_stored.sbit) {
      switch (_ihdr.color_type) {
        case PNG_COLOR_TYPE_GRAYSCALE: {
          length = 1;

          data[0] = _sbit.grayscale_bits;

          break;
        }
        case PNG_COLOR_TYPE_TRUECOLOR:
        case PNG_COLOR_TYPE_INDEXED: {
          length = 3;

          data[0] = _sbit.red_bits;
          data[1] = _sbit.green_bits;
          data[2] = _sbit.blue_bits;

          break;
        }
        case PNG_COLOR_TYPE_GRAYSCALE_ALPHA: {
          length = 2;

          data[0] = _sbit.grayscale_bits;
          data[1] = _sbit.alpha_bits;

          break;
        }
        case PNG_COLOR_TYPE_TRUECOLOR_ALPHA: {
          length = 4;

          data[0] = _sbit.red_bits;
          data[1] = _sbit.green_bits;
          data[2] = _sbit.blue_bits;
          data[3] = _sbit.alpha_bits;

          break;
        }
        default:
          return PNG_EINTERNAL;
      }

      ret = write_chunk(type_sbit, data, length);
      if (ret) return ret;
    }

    if (_stored.srgb) {
      ret = write_chunk(type_srgb, &_srgb_rendering_intent, 1);
      if (ret) return ret;
    }

    ret = write_unknown_chunks(PNG_AFTER_IHDR);
    if (ret) return ret;

    if (_stored.plte) {
      for (i = 0; i < _plte.n_entries; i++) {
        data[i * 3 + 0] = _plte.entries[i].red;
        data[i * 3 + 1] = _plte.entries[i].green;
        data[i * 3 + 2] = _plte.entries[i].blue;
      }

      ret = write_chunk(type_plte, data, _plte.n_entries * 3);
      if (ret) return ret;
    }

    if (_stored.bkgd) {
      switch (_ihdr.color_type) {
        case PNG_COLOR_TYPE_GRAYSCALE:
        case PNG_COLOR_TYPE_GRAYSCALE_ALPHA: {
          length = 2;

          write_u16(data, _bkgd.gray);

          break;
        }
        case PNG_COLOR_TYPE_TRUECOLOR:
        case PNG_COLOR_TYPE_TRUECOLOR_ALPHA: {
          length = 6;

          write_u16(data, _bkgd.red);
          write_u16(data + 2, _bkgd.green);
          write_u16(data + 4, _bkgd.blue);

          break;
        }
        case PNG_COLOR_TYPE_INDEXED: {
          length = 1;

          data[0] = _bkgd.plte_index;

          break;
        }
        default:
          return PNG_EINTERNAL;
      }

      ret = write_chunk(type_bkgd, data, length);
      if (ret) return ret;
    }

    if (_stored.hist) {
      length = _plte.n_entries * 2;

      for (i = 0; i < _plte.n_entries; i++) {
        write_u16(data + i * 2, _hist.frequency[i]);
      }

      ret = write_chunk(type_hist, data, length);
      if (ret) return ret;
    }

    if (_stored.trns) {
      if (_ihdr.color_type == PNG_COLOR_TYPE_GRAYSCALE) {
        write_u16(data, _trns.gray);

        ret = write_chunk(type_trns, data, 2);
      } else if (_ihdr.color_type == PNG_COLOR_TYPE_TRUECOLOR) {
        write_u16(data, _trns.red);
        write_u16(data + 2, _trns.green);
        write_u16(data + 4, _trns.blue);

        ret = write_chunk(type_trns, data, 6);
      } else if (_ihdr.color_type == PNG_COLOR_TYPE_INDEXED) {
        ret = write_chunk(type_trns, _trns.type3_alpha, _trns.n_type3_entries);
      }

      if (ret) return ret;
    }

    if (_stored.phys) {
      write_u32(data, _phys.ppu_x);
      write_u32(data + 4, _phys.ppu_y);
      data[8] = _phys.unit_specifier;

      ret = write_chunk(type_phys, data, 9);
      if (ret) return ret;
    }

    if (_stored.splt) {
      const struct SPLT *splt;
      unsigned char *cdata = nullptr;

      uint32_t k;
      for (i = 0; i < _n_splt; i++) {
        splt = &_splt_list[i];

        size_t name_len = strlen(splt->name);
        length = name_len + 1;

        if (splt->sample_depth == 8) length += splt->n_entries * 6 + 1;
        else if (splt->sample_depth == 16) length += splt->n_entries * 10 + 1;

        ret = write_header(type_splt, length, &cdata);
        if (ret) return ret;

        memcpy(cdata, splt->name, name_len + 1);
        cdata += name_len + 2;
        cdata[-1] = splt->sample_depth;

        if (splt->sample_depth == 8) {
          for (k = 0; k < splt->n_entries; k++) {
            cdata[k * 6 + 0] = splt->entries[k].red;
            cdata[k * 6 + 1] = splt->entries[k].green;
            cdata[k * 6 + 2] = splt->entries[k].blue;
            cdata[k * 6 + 3] = splt->entries[k].alpha;
            write_u16(cdata + k * 6 + 4, splt->entries[k].frequency);
          }
        } else if (splt->sample_depth == 16) {
          for (k = 0; k < splt->n_entries; k++) {
            write_u16(cdata + k * 10 + 0, splt->entries[k].red);
            write_u16(cdata + k * 10 + 2, splt->entries[k].green);
            write_u16(cdata + k * 10 + 4, splt->entries[k].blue);
            write_u16(cdata + k * 10 + 6, splt->entries[k].alpha);
            write_u16(cdata + k * 10 + 8, splt->entries[k].frequency);
          }
        }

        ret = finish_chunk();
        if (ret) return ret;
      }
    }

    if (_stored.time) {
      write_u16(data, _time.year);
      data[2] = _time.month;
      data[3] = _time.day;
      data[4] = _time.hour;
      data[5] = _time.minute;
      data[6] = _time.second;

      ret = write_chunk(type_time, data, 7);
      if (ret) return ret;
    }

    if (_stored.text) {
      unsigned char *cdata = nullptr;
      const struct Text2 *text;
      const uint8_t *text_type_array[4] = {0, type_text, type_ztxt, type_itxt};

      for (i = 0; i < _n_text; i++) {
        text = &_text_list[i];

        const uint8_t *text_chunk_type = text_type_array[text->type];
        byte *compressed_text = nullptr;
        size_t keyword_len = 0;
        size_t language_tag_len = 0;
        size_t translated_keyword_len = 0;
        size_t compressed_length = 0;
        size_t text_length = 0;

        keyword_len = strlen(text->keyword);
        text_length = strlen(text->text);

        length = keyword_len + 1;

        if (text->type == PNG_ZTXT) {
          length += 1; /* compression method */
        } else if (text->type == PNG_ITXT) {
          if (!text->language_tag || !text->translated_keyword) return PNG_EINTERNAL;

          language_tag_len = strlen(text->language_tag);
          translated_keyword_len = strlen(text->translated_keyword);

          length += language_tag_len;
          if (length < language_tag_len) return PNG_EOVERFLOW;

          length += translated_keyword_len;
          if (length < translated_keyword_len) return PNG_EOVERFLOW;

          length += 4; /* compression flag + method + nul for the two strings */
          if (length < 4) return PNG_EOVERFLOW;
        }

        if (text->compression_flag) {
          ret = deflate_init(_text_options);
          if (ret) return ret;

          gccucore::zlib::z_stream *zstream = &_zstream;
          unsigned long dest_len = gccucore::zlib::deflateBound(zstream, (unsigned long) text_length);

          compressed_text = (byte *) malloc(dest_len);

          if (compressed_text == nullptr) return PNG_EMEM;

          zstream->next_in = (const byte *) text->text;
          zstream->avail_in = (uint32_t) text_length;

          zstream->next_out = compressed_text;
          zstream->avail_out = dest_len;

          ret = gccucore::zlib::deflate(zstream, Z_FINISH);

          if (ret != Z_STREAM_END) {
            free(compressed_text);
            return PNG_EZLIB;
          }

          compressed_length = zstream->total_out;

          length += compressed_length;
          if (length < compressed_length) return PNG_EOVERFLOW;
        } else {
          text_length = strlen(text->text);

          length += text_length;
          if (length < text_length) return PNG_EOVERFLOW;
        }

        ret = write_header(text_chunk_type, length, &cdata);
        if (ret) {
          free(compressed_text);
          return ret;
        }

        memcpy(cdata, text->keyword, keyword_len + 1);
        cdata += keyword_len + 1;

        if (text->type == PNG_ITXT) {
          cdata[0] = text->compression_flag;
          cdata[1] = 0; /* compression method */
          cdata += 2;

          memcpy(cdata, text->language_tag, language_tag_len + 1);
          cdata += language_tag_len + 1;

          memcpy(cdata, text->translated_keyword, translated_keyword_len + 1);
          cdata += translated_keyword_len + 1;
        } else if (text->type == PNG_ZTXT) {
          cdata[0] = 0; /* compression method */
          cdata++;
        }

        if (text->compression_flag) memcpy(cdata, compressed_text, compressed_length);
        else memcpy(cdata, text->text, text_length);

        free(compressed_text);

        ret = finish_chunk();
        if (ret) return ret;
      }
    }

    if (_stored.offs) {
      write_s32(data, _offs.x);
      write_s32(data + 4, _offs.y);
      data[8] = _offs.unit_specifier;

      ret = write_chunk(type_offs, data, 9);
      if (ret) return ret;
    }

    if (_stored.exif) {
      ret = write_chunk(type_exif, _exif.data, _exif.length);
      if (ret) return ret;
    }

    ret = write_unknown_chunks(PNG_AFTER_PLTE);
    if (ret) return ret;

    return 0;
  }

  int PngParser::write_chunks_after_idat() {
    int ret = write_unknown_chunks(PNG_AFTER_IDAT);
    if (ret) return ret;

    return write_iend();
  }

/* Compress and write scanline to IDAT stream */
  int PngParser::write_idat_bytes(const void *scanline, size_t len, int flush) {
    if (scanline == nullptr) return PNG_EINTERNAL;
    if (len > UINT_MAX) return PNG_EINTERNAL;

    int ret = 0;
    unsigned char *data = nullptr;
    gccucore::zlib::z_stream *zstream = &_zstream;
    uint32_t idat_length = PNG_WRITE_SIZE;

    zstream->next_in = (const byte *) scanline;
    zstream->avail_in = (uint32_t) len;

    do {
      ret = gccucore::zlib::deflate(zstream, flush);

      if (zstream->avail_out == 0) {
        ret = finish_chunk();
        if (ret) return on_error(ret);

        ret = write_header(type_idat, idat_length, &data);
        if (ret) return on_error(ret);

        zstream->next_out = data;
        zstream->avail_out = idat_length;
      }

    } while (zstream->avail_in);

    if (ret != Z_OK) return PNG_EZLIB;

    return 0;
  }

  int PngParser::finish_idat() {
    int ret = 0;
    unsigned char *data = nullptr;
    gccucore::zlib::z_stream *zstream = &_zstream;
    uint32_t idat_length = PNG_WRITE_SIZE;

    while (ret != Z_STREAM_END) {
      ret = gccucore::zlib::deflate(zstream, Z_FINISH);

      if (ret) {
        if (ret == Z_STREAM_END) break;

        if (ret != Z_BUF_ERROR) return PNG_EZLIB;
      }

      if (zstream->avail_out == 0) {
        ret = finish_chunk();
        if (ret) return on_error(ret);

        ret = write_header(type_idat, idat_length, &data);
        if (ret) return on_error(ret);

        zstream->next_out = data;
        zstream->avail_out = idat_length;
      }
    }

    uint32_t trimmed_length = idat_length - zstream->avail_out;

    ret = trim_chunk(trimmed_length);
    if (ret) return ret;

    return finish_chunk();
  }

  int PngParser::encode_scanline(const void *scanline, size_t len) {
    if (scanline == nullptr) return PNG_EINTERNAL;

    int ret, pass = _row_info.pass;
    uint8_t filter = 0;
    struct RowInfo *ri = &_row_info;
    const struct SubImage *sub = _subimage;
    struct EncodeFlagStat f = _encode_flags;
    unsigned char *filtered_scanline = _filtered_scanline;
    size_t scanline_width = sub[pass].scanline_width;

    if (len < scanline_width - 1) return PNG_EINTERNAL;

    /* encode_row() interlaces directly to _scanline */
    if (scanline != _scanline) memcpy(_scanline, scanline, scanline_width - 1);

    if (f.to_bigendian) u16_row_to_bigendian(_scanline, scanline_width - 1);
    const int requires_previous =
        f.filter_choice & (PNG_FILTER_CHOICE_UP | PNG_FILTER_CHOICE_AVG | PNG_FILTER_CHOICE_PAETH);

    /* XXX: exclude 'requires_previous' filters by default for first scanline? */
    if (!ri->scanline_idx && requires_previous) {
      /* prev_scanline is all zeros for the first scanline */
      memset(_prev_scanline, 0, scanline_width);
    }

    filter = get_best_filter(_prev_scanline, _scanline, scanline_width, _bytes_per_pixel,
                             f.filter_choice);

    if (!filter) filtered_scanline = _scanline;

    filtered_scanline[-1] = filter;

    if (filter) {
      ret = filter_scanline(filtered_scanline, _prev_scanline, _scanline, scanline_width,
                            _bytes_per_pixel,
                            filter);
      if (ret) return on_error(ret);
    }

    ret = write_idat_bytes(filtered_scanline - 1, scanline_width, Z_NO_FLUSH);
    if (ret) return on_error(ret);

    /* The previous scanline is always unfiltered */
    auto t = _prev_scanline;
    _prev_scanline = _scanline;
    _scanline = t;

    ret = update_row_info();

    if (ret == PNG_EOI) {
      int error = finish_idat();
      if (error) on_error(error);

      if (f.finalize) {
        error = encode_chunks();
        if (error) return on_error(error);
      }
    }

    return ret;
  }

  int PngParser::encode_row(const void *row, size_t len) {
    if (row == nullptr) return PNG_EINTERNAL;

    const int pass = _row_info.pass;

    if (!_ihdr.interlace_method || pass == 6) return encode_scanline(row, len);

    uint32_t k;
    const unsigned pixel_size = _pixel_size;
    const unsigned bit_depth = _ihdr.bit_depth;

    if (bit_depth < 8) {
      const unsigned samples_per_byte = 8 / bit_depth;
      const uint8_t mask = (1 << bit_depth) - 1;
      const unsigned initial_shift = 8 - bit_depth;
      unsigned shift_amount = initial_shift;

      unsigned char *scanline = _scanline;
      auto row_uc = (const byte *) row;
      uint8_t sample;

      memset(scanline, 0, _subimage[pass].scanline_width);

      for (k = 0; k < _subimage[pass].width; k++) {
        size_t ioffset = adam7_x_start[pass] + k * adam7_x_delta[pass];

        sample = row_uc[ioffset / samples_per_byte];

        sample = sample >> (initial_shift - ioffset * bit_depth % 8);
        sample = sample & mask;
        sample = sample << shift_amount;

        scanline[0] |= sample;

        shift_amount -= bit_depth;

        if (shift_amount > 7) {
          shift_amount = initial_shift;
          scanline++;
        }
      }

      return encode_scanline(_scanline, len);
    }

    for (k = 0; k < _subimage[pass].width; k++) {
      size_t ioffset = (adam7_x_start[pass] + (size_t) k * adam7_x_delta[pass]) * pixel_size;

      memcpy(_scanline + k * pixel_size, (unsigned char *) row + ioffset, pixel_size);
    }

    return encode_scanline(_scanline, len);
  }

  int PngParser::encode_chunks() {
    if (!_state) return PNG_EBADSTATE;
    if (_state < PNG_STATE_OUTPUT) return PNG_ENODST;
    if (!_encode_only) return PNG_ECTXTYPE;

    int ret = 0;

    if (_state < PNG_STATE_FIRST_IDAT) {
      if (!_stored.ihdr) return PNG_ENOIHDR;

      ret = write_chunks_before_idat();
      if (ret) return on_error(ret);

      _state = PNG_STATE_FIRST_IDAT;
    } else if (_state == PNG_STATE_FIRST_IDAT) {
      return 0;
    } else if (_state == PNG_STATE_EOI) {
      ret = write_chunks_after_idat();
      if (ret) return on_error(ret);

      _state = PNG_STATE_IEND;
    } else return PNG_EOPSTATE;

    return 0;
  }

  int PngParser::encode_image(const void *img, size_t len, Format fmt, int flags) {
    if (!_state) return PNG_EBADSTATE;
    if (!_encode_only) return PNG_ECTXTYPE;
    if (!_stored.ihdr) return PNG_ENOIHDR;
    if (!(fmt == PNG_FMT_PNG || fmt == PNG_FMT_RAW)) return PNG_EFMT;

    int ret = 0;
    const iHeader *ihdr = &_ihdr;
    struct EncodeFlagStat *encode_flags = &_encode_flags;

    if (ihdr->color_type == PNG_COLOR_TYPE_INDEXED && !_stored.plte) return PNG_ENOPLTE;

    ret = ihdr->calculate_image_width(fmt, _image_width);
    if (ret) return on_error(ret);

    if (_image_width > SIZE_MAX / ihdr->height) _image_size = 0; /* overflow */
    else _image_size = _image_width * ihdr->height;

    if (!(flags & PNG_ENCODE_PROGRESSIVE)) {
      if (img == nullptr) return 1;
      if (!_image_size) return PNG_EOVERFLOW;
      if (len != _image_size) return PNG_EBUFSIZ;
    }

    ret = encode_chunks();
    if (ret) return on_error(ret);

    ret = calculate_subimages();
    if (ret) return on_error(ret);

    if (ihdr->bit_depth < 8) _bytes_per_pixel = 1;
    else _bytes_per_pixel = ihdr->num_channels() * (ihdr->bit_depth / 8);

    if (_optimize_option & (1 << PNG_FILTER_CHOICE)) {
      /* Filtering would make no difference */
      if (!_image_options._compressionLevel) {
        encode_flags->filter_choice = PNG_DISABLE_FILTERING;
      }

      /* Palette indices and low bit-depth images do not benefit from filtering */
      if (ihdr->color_type == PNG_COLOR_TYPE_INDEXED || ihdr->bit_depth < 8) {
        encode_flags->filter_choice = PNG_DISABLE_FILTERING;
      }
    }

    /* This is technically the same as disabling filtering */
    if (encode_flags->filter_choice == PNG_FILTER_CHOICE_NONE) {
      encode_flags->filter_choice = PNG_DISABLE_FILTERING;
    }

    if (!encode_flags->filter_choice && _optimize_option & (1 << PNG_IMG_COMPRESSION_STRATEGY)) {
      _image_options._strategy = Z_DEFAULT_STRATEGY;
    }

    ret = deflate_init(_image_options);
    if (ret) return on_error(ret);

    size_t scanline_buf_size = _subimage[_widest_pass].scanline_width;

    scanline_buf_size += 32;

    if (scanline_buf_size < 32) return PNG_EOVERFLOW;

    _scanline_buf = (byte *) malloc(scanline_buf_size);
    _prev_scanline_buf = (byte *) malloc(scanline_buf_size);

    if (_scanline_buf == nullptr || _prev_scanline_buf == nullptr) return on_error(PNG_EMEM);

    /* Maintain alignment for pixels, filter at [-1] */
    _scanline = _scanline_buf + 16;
    _prev_scanline = _prev_scanline_buf + 16;

    if (encode_flags->filter_choice) {
      _filtered_scanline_buf = (byte *) malloc(scanline_buf_size);
      if (_filtered_scanline_buf == nullptr) return on_error(PNG_EMEM);

      _filtered_scanline = _filtered_scanline_buf + 16;
    }

    struct SubImage *sub = _subimage;
    struct RowInfo *ri = &_row_info;

    _fmt = fmt;

    gccucore::zlib::z_stream *zstream = &_zstream;
    zstream->avail_out = PNG_WRITE_SIZE;

    ret = write_header(type_idat, zstream->avail_out, &zstream->next_out);
    if (ret) return on_error(ret);

    if (ihdr->interlace_method) encode_flags->interlace = 1;

    if (fmt & (PNG_FMT_PNG | PNG_FMT_RAW)) encode_flags->same_layout = 1;

    if (ihdr->bit_depth == 16 && fmt != PNG_FMT_RAW) encode_flags->to_bigendian = 1;

    if (flags & PNG_ENCODE_FINALIZE) encode_flags->finalize = 1;

    while (!sub[ri->pass].width || !sub[ri->pass].height) ri->pass++;

    if (encode_flags->interlace) ri->row_num = adam7_y_start[ri->pass];

    _pixel_size = 4; /* PNG_FMT_RGBA8 */

    if (fmt == PNG_FMT_RGBA16) _pixel_size = 8;
    else if (fmt == PNG_FMT_RGB8) _pixel_size = 3;
    else if (fmt & (PNG_FMT_PNG | PNG_FMT_RAW)) _pixel_size = _bytes_per_pixel;

    _state = PNG_STATE_ENCODE_INIT;

    if (flags & PNG_ENCODE_PROGRESSIVE) {
      encode_flags->progressive = 1;

      return 0;
    }

    do {
      size_t ioffset = ri->row_num * _image_width;

      ret = encode_row((unsigned char *) img + ioffset, _image_width);

    } while (!ret);

    if (ret != PNG_EOI) return on_error(ret);

    return 0;
  }

  static int file_read_fn(PngParser *ctx, void *user, void *data, size_t n) {
    auto file = (FILE *) user;
    (void) ctx;

    if (fread(data, n, 1, file) != 1) {
      if (feof(file)) return PngParser::PNG_IO_EOF;
      else return PngParser::PNG_IO_ERROR;
    }

    return 0;
  }

  static int file_write_fn(PngParser *ctx, void *user, void *data, size_t n) {
    auto file = (FILE *) user;
    (void) ctx;

    if (fwrite(data, n, 1, file) != 1) return PngParser::PNG_IO_ERROR;

    return 0;
  }

  int PngParser::set_png_stream(_rw_fn_ptr *rw_func, void *user) {
    if (rw_func == nullptr) return 1;
    if (!_state) return PNG_EBADSTATE;

    /* PNG_STATE_OUTPUT shares the same value */
    if (_state >= PNG_STATE_INPUT) return PNG_EBUF_SET;

    if (_encode_only) {
      if (_out_png != nullptr) return PNG_EBUF_SET;

      _write_fn = rw_func;
      _write_ptr = _stream_buf;

      _state = PNG_STATE_OUTPUT;
    } else {
      _stream_buf = (byte *) malloc(PNG_READ_SIZE);
      if (_stream_buf == nullptr) return PNG_EMEM;

      _read_fn = rw_func;
      _data = _stream_buf;

      _state = PNG_STATE_INPUT;
    }

    _stream_user_ptr = user;

    _streaming = 1;

    return 0;
  }

  int PngParser::set_png_file(FILE *file) {
    if (file == nullptr) return 1;

    if (_encode_only) return set_png_stream(file_write_fn, file);

    return set_png_stream(file_read_fn, file);
  }

  void *PngParser::get_png_buffer(size_t *len, int *error) {
    int tmp = 0;
    error = error ? error : &tmp;
    *error = 0;

    if (!len) *error = PNG_EINVAL;

    if (*error) return nullptr;

    if (!_encode_only) *error = PNG_ECTXTYPE;
    else if (!_state) *error = PNG_EBADSTATE;
    else if (!_internal_buffer) *error = PNG_EOPSTATE;
    else if (_state < PNG_STATE_EOI) *error = PNG_EOPSTATE;
    else if (_state != PNG_STATE_IEND) *error = PNG_ENOTFINAL;

    if (*error) return nullptr;

    _user_owns_out_png = 1;

    *len = _bytes_encoded;

    return _out_png;
  }

  int PngParser::set_image_limits(uint32_t width, uint32_t height) {
    if (width > _u32max || height > _u32max) return 1;

    _max_width = width;
    _max_height = height;

    return 0;
  }

  int PngParser::get_image_limits(uint32_t *width, uint32_t *height) {
    if (width == nullptr || height == nullptr) return 1;

    *width = _max_width;
    *height = _max_height;

    return 0;
  }

  int PngParser::set_chunk_limits(size_t chunk_size, size_t cache_limit) {
    if (chunk_size > _u32max || chunk_size > cache_limit) return 1;

    _max_chunk_size = chunk_size;
    _chunk_cache_limit = cache_limit;

    return 0;
  }

  int PngParser::get_chunk_limits(size_t *chunk_size, size_t *cache_limit) {
    if (chunk_size == nullptr || cache_limit == nullptr) return 1;

    *chunk_size = _max_chunk_size;

    *cache_limit = _chunk_cache_limit;

    return 0;
  }

  int PngParser::set_crc_action(int critical, int ancillary) {
    if (_encode_only) return PNG_ECTXTYPE;

    if (critical > 2 || critical < 0) return 1;
    if (ancillary > 2 || ancillary < 0) return 1;

    if (critical == PNG_CRC_DISCARD) return 1;

    _crc_action_critical = critical;
    _crc_action_ancillary = ancillary;

    return 0;
  }

  int PngParser::set_option(Option option, int value) {
    if (!_state) return PNG_EBADSTATE;

    switch (option) {
      case PNG_KEEP_UNKNOWN_CHUNKS: {
        _keep_unknown = value ? 1 : 0;
        break;
      }
      case PNG_IMG_COMPRESSION_LEVEL: {
        _image_options._compressionLevel = value;
        break;
      }
      case PNG_IMG_WINDOW_BITS: {
        _image_options._windowBits = value;
        break;
      }
      case PNG_IMG_MEM_LEVEL: {
        _image_options._memLevel = value;
        break;
      }
      case PNG_IMG_COMPRESSION_STRATEGY: {
        _image_options._strategy = value;
        break;
      }
      case PNG_TEXT_COMPRESSION_LEVEL: {
        _text_options._compressionLevel = value;
        break;
      }
      case PNG_TEXT_WINDOW_BITS: {
        _text_options._windowBits = value;
        break;
      }
      case PNG_TEXT_MEM_LEVEL: {
        _text_options._memLevel = value;
        break;
      }
      case PNG_TEXT_COMPRESSION_STRATEGY: {
        _text_options._strategy = value;
        break;
      }
      case PNG_FILTER_CHOICE: {
        if (value & ~PNG_FILTER_CHOICE_ALL) return 1;
        _encode_flags.filter_choice = (FilterChoice) value;
        break;
      }
      case PNG_CHUNK_COUNT_LIMIT: {
        if (value < 0) return 1;
        if (value > (int) _chunk_count_total) return 1;
        _chunk_count_limit = value;
        break;
      }
      case PNG_ENCODE_TO_BUFFER: {
        if (value < 0) return 1;
        if (!_encode_only) return PNG_ECTXTYPE;
        if (_state >= PNG_STATE_OUTPUT) return PNG_EOPSTATE;

        if (!value) break;

        _internal_buffer = 1;
        _state = PNG_STATE_OUTPUT;

        break;
      }
      default:
        return 1;
    }

    /* Option can no longer be overriden by the library */
    if (option < 32) _optimize_option &= ~(1 << option);

    return 0;
  }

  int PngParser::get_option(Option option, int *value) {
    if (value == nullptr) return 1;
    if (!_state) return PNG_EBADSTATE;

    switch (option) {
      case PNG_KEEP_UNKNOWN_CHUNKS: {
        *value = _keep_unknown;
        break;
      }
      case PNG_IMG_COMPRESSION_LEVEL: {
        *value = _image_options._compressionLevel;
        break;
      }
      case PNG_IMG_WINDOW_BITS: {
        *value = _image_options._windowBits;
        break;
      }
      case PNG_IMG_MEM_LEVEL: {
        *value = _image_options._memLevel;
        break;
      }
      case PNG_IMG_COMPRESSION_STRATEGY: {
        *value = _image_options._strategy;
        break;
      }
      case PNG_TEXT_COMPRESSION_LEVEL: {
        *value = _text_options._compressionLevel;
        break;
      }
      case PNG_TEXT_WINDOW_BITS: {
        *value = _text_options._windowBits;
        break;
      }
      case PNG_TEXT_MEM_LEVEL: {
        *value = _text_options._memLevel;
        break;
      }
      case PNG_TEXT_COMPRESSION_STRATEGY: {
        *value = _text_options._strategy;
        break;
      }
      case PNG_FILTER_CHOICE: {
        *value = _encode_flags.filter_choice;
        break;
      }
      case PNG_CHUNK_COUNT_LIMIT: {
        *value = _chunk_count_limit;
        break;
      }
      case PNG_ENCODE_TO_BUFFER: {
        if (_internal_buffer) *value = 1;
        else *value = 0;

        break;
      }
      default:
        return 1;
    }

    return 0;
  }

  int PngParser::decoded_image_size(int fmt, size_t& len) {
    int ret = check_decode_fmt(fmt);
    if (ret) return ret;

    return _ihdr.calculate_image_size(fmt, len);
  }

  PngParser::iHeader PngParser::get_ihdr() {
    return _ihdr;
  }

  struct PngParser::PLTE PngParser::get_plte() {
    PNG_GET_CHUNK_BOILERPLATE(plte);
    return _plte;
  }

  struct PngParser::Trns PngParser::get_trns() {
    PNG_GET_CHUNK_BOILERPLATE(trns);
    return _trns;
  }

  struct PngParser::Chrm PngParser::get_chrm() {
    PNG_GET_CHUNK_BOILERPLATE(chrm);

    struct Chrm chrm;
    chrm.white_point_x = (double) _chrm_int.white_point_x / 100000.0;
    chrm.white_point_y = (double) _chrm_int.white_point_y / 100000.0;
    chrm.red_x = (double) _chrm_int.red_x / 100000.0;
    chrm.red_y = (double) _chrm_int.red_y / 100000.0;
    chrm.blue_y = (double) _chrm_int.blue_y / 100000.0;
    chrm.blue_x = (double) _chrm_int.blue_x / 100000.0;
    chrm.green_x = (double) _chrm_int.green_x / 100000.0;
    chrm.green_y = (double) _chrm_int.green_y / 100000.0;

    return chrm;
  }

  struct PngParser::ChrmInt PngParser::get_chrm_int() {
    PNG_GET_CHUNK_BOILERPLATE(chrm);
    return _chrm_int;
  }

  double PngParser::get_gama() {
    PNG_GET_CHUNK_BOILERPLATE(gama);
    return (double) _gama / 100000.0;
  }

  uint32_t PngParser::get_gama_int() {
    PNG_GET_CHUNK_BOILERPLATE(gama);
    return _gama;
  }

  struct PngParser::ICCP PngParser::get_iccp() {
    PNG_GET_CHUNK_BOILERPLATE(iccp);
    return _iccp;
  }

  struct PngParser::SBIT PngParser::get_sbit() {
    PNG_GET_CHUNK_BOILERPLATE(sbit);
    return _sbit;
  }

  uint8_t PngParser::get_srgb() {
    PNG_GET_CHUNK_BOILERPLATE(srgb);
    return _srgb_rendering_intent;
  }

  upan::vector<struct PngParser::TEXT> PngParser::get_text() {
    PNG_GET_CHUNK_BOILERPLATE(text);

    upan::vector<struct PngParser::TEXT> vText;
    for (uint32_t i = 0; i < _n_text; i++) {
      struct PngParser::TEXT text;
      text.type = _text_list[i].type;
      memcpy(&text.keyword, _text_list[i].keyword, strlen(_text_list[i].keyword) + 1);
      text.compression_method = 0;
      text.compression_flag = _text_list[i].compression_flag;
      text.language_tag = _text_list[i].language_tag;
      text.translated_keyword = _text_list[i].translated_keyword;
      text.length = _text_list[i].text_length;
      text.text = _text_list[i].text;
      vText.push_back(text);
    }

    return vText;
  }

  struct PngParser::BKGD PngParser::get_bkgd() {
    PNG_GET_CHUNK_BOILERPLATE(bkgd);
    return _bkgd;
  }

  struct PngParser::HIST PngParser::get_hist() {
    PNG_GET_CHUNK_BOILERPLATE(hist);
    return _hist;
  }

  struct PngParser::PHYS PngParser::get_phys() {
    PNG_GET_CHUNK_BOILERPLATE(phys);
    return _phys;
  }

  int PngParser::get_splt(struct SPLT *splt, uint32_t *n_splt) {
    if (!_stored.splt) return PNG_ECHUNKAVAIL;
    if (n_splt == nullptr) return 1;

    if (splt == nullptr) {
      *n_splt = _n_splt;
      return 0;
    }

    if (*n_splt < _n_splt) return 1;

    memcpy(splt, _splt_list, _n_splt * sizeof(struct SPLT));

    return 0;
  }

  struct PngParser::Time PngParser::get_time() {
    PNG_GET_CHUNK_BOILERPLATE(time);
    return _time;
  }

  int PngParser::get_unknown_chunks(struct UnknownChunk *chunks, uint32_t *n_chunks) {
    if (!_stored.unknown) return PNG_ECHUNKAVAIL;
    if (n_chunks == nullptr) return 1;

    if (chunks == nullptr) {
      *n_chunks = _n_chunks;
      return 0;
    }

    if (*n_chunks < _n_chunks) return 1;

    memcpy(chunks, _chunk_list, sizeof(struct UnknownChunk));

    return 0;
  }

  struct PngParser::Offs PngParser::get_offs() {
    PNG_GET_CHUNK_BOILERPLATE(offs);
    return _offs;
  }

  struct PngParser::Exif PngParser::get_exif() {
    PNG_GET_CHUNK_BOILERPLATE(exif);
    return _exif;
  }

  int PngParser::set_ihdr(const iHeader &ihdr) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (_stored.ihdr) return 1;

    int ret = check_ihdr(ihdr, _max_width, _max_height);
    if (ret) return ret;

    _ihdr = ihdr;

    _stored.ihdr = 1;
    _user.ihdr = 1;

    return 0;
  }

  int PngParser::set_plte(const struct PLTE &plte) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (!_stored.ihdr) return 1;

    if (check_plte(plte, _ihdr)) return 1;

    _plte.n_entries = plte.n_entries;

    memcpy(_plte.entries, plte.entries, plte.n_entries * sizeof(struct PLTEEntry));

    _stored.plte = 1;
    _user.plte = 1;

    return 0;
  }

  int PngParser::set_trns(const struct Trns &trns) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (!_stored.ihdr) return PNG_ENOIHDR;

    if (_ihdr.color_type == PNG_COLOR_TYPE_GRAYSCALE) {
      _trns.gray = trns.gray;
    } else if (_ihdr.color_type == PNG_COLOR_TYPE_TRUECOLOR) {
      _trns.red = trns.red;
      _trns.green = trns.green;
      _trns.blue = trns.blue;
    } else if (_ihdr.color_type == PNG_COLOR_TYPE_INDEXED) {
      if (!_stored.plte) return PNG_ETRNS_NO_PLTE;
      if (trns.n_type3_entries > _plte.n_entries) return 1;

      _trns.n_type3_entries = trns.n_type3_entries;
      memcpy(_trns.type3_alpha, trns.type3_alpha, trns.n_type3_entries);
    } else return PNG_ETRNS_COLOR_TYPE;

    _stored.trns = 1;
    _user.trns = 1;

    return 0;
  }

  int PngParser::set_chrm(const struct Chrm &chrm) {
    PNG_SET_CHUNK_BOILERPLATE();

    struct ChrmInt chrm_int;

    chrm_int.white_point_x = (uint32_t) (chrm.white_point_x * 100000.0);
    chrm_int.white_point_y = (uint32_t) (chrm.white_point_y * 100000.0);
    chrm_int.red_x = (uint32_t) (chrm.red_x * 100000.0);
    chrm_int.red_y = (uint32_t) (chrm.red_y * 100000.0);
    chrm_int.green_x = (uint32_t) (chrm.green_x * 100000.0);
    chrm_int.green_y = (uint32_t) (chrm.green_y * 100000.0);
    chrm_int.blue_x = (uint32_t) (chrm.blue_x * 100000.0);
    chrm_int.blue_y = (uint32_t) (chrm.blue_y * 100000.0);

    if (check_chrm_int(chrm_int)) return PNG_ECHRM;

    _chrm_int = chrm_int;

    _stored.chrm = 1;
    _user.chrm = 1;

    return 0;
  }

  int PngParser::set_chrm_int(const struct ChrmInt &chrm_int) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (check_chrm_int(chrm_int)) return PNG_ECHRM;

    _chrm_int = chrm_int;

    _stored.chrm = 1;
    _user.chrm = 1;

    return 0;
  }

  int PngParser::set_gama(double gamma) {
    PNG_SET_CHUNK_BOILERPLATE();

    uint32_t gama = gamma * 100000.0;

    if (!gama) return 1;
    if (gama > _u32max) return 1;

    _gama = gama;

    _stored.gama = 1;
    _user.gama = 1;

    return 0;
  }

  int PngParser::set_gama_int(uint32_t gamma) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (!gamma) return 1;
    if (gamma > _u32max) return 1;

    _gama = gamma;

    _stored.gama = 1;
    _user.gama = 1;

    return 0;
  }

  int PngParser::set_iccp(const struct ICCP &iccp) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (check_png_keyword(iccp.profile_name)) return PNG_EICCP_NAME;
    if (!iccp.profile_len) return PNG_ECHUNK_SIZE;
    if (iccp.profile_len > _u32max) return PNG_ECHUNK_STDLEN;

    if (_iccp.profile && !_user.iccp) free(_iccp.profile);

    _iccp = iccp;

    _stored.iccp = 1;
    _user.iccp = 1;

    return 0;
  }

  int PngParser::set_sbit(const struct SBIT &sbit) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (check_sbit(sbit, _ihdr)) return 1;

    if (!_stored.ihdr) return 1;

    _sbit = sbit;

    _stored.sbit = 1;
    _user.sbit = 1;

    return 0;
  }

  int PngParser::set_srgb(uint8_t rendering_intent) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (rendering_intent > 3) return 1;

    _srgb_rendering_intent = rendering_intent;

    _stored.srgb = 1;
    _user.srgb = 1;

    return 0;
  }

  int PngParser::set_text(const struct TEXT *text, uint32_t n_text) {
    if (!n_text || !text) return 1;
    PNG_SET_CHUNK_BOILERPLATE();

    uint32_t i;
    for (i = 0; i < n_text; i++) {
      if (check_png_keyword(text[i].keyword)) return PNG_ETEXT_KEYWORD;
      if (!text[i].length) return 1;
      if (text[i].length > UINT_MAX) return 1;
      if (text[i].text == nullptr) return 1;

      if (text[i].type == PNG_TEXT) {
        if (_strict && check_png_text(text[i].text, text[i].length)) return 1;
      } else if (text[i].type == PNG_ZTXT) {
        if (_strict && check_png_text(text[i].text, text[i].length)) return 1;

        if (text[i].compression_method != 0) return PNG_EZTXT_COMPRESSION_METHOD;
      } else if (text[i].type == PNG_ITXT) {
        if (text[i].compression_flag > 1) return PNG_EITXT_COMPRESSION_FLAG;
        if (text[i].compression_method != 0) return PNG_EITXT_COMPRESSION_METHOD;
        if (text[i].language_tag == nullptr) return PNG_EITXT_LANG_TAG;
        if (text[i].translated_keyword == nullptr) return PNG_EITXT_TRANSLATED_KEY;
      } else return 1;

    }

    auto text_list = (struct Text2 *) calloc(sizeof(struct Text2), n_text);

    if (!text_list) return PNG_EMEM;

    if (_text_list != nullptr) {
      for (i = 0; i < _n_text; i++) {
        if (_user.text) break;

        free(_text_list[i].keyword);
        if (_text_list[i].compression_flag) free(_text_list[i].text);
      }
      free(_text_list);
    }

    for (i = 0; i < n_text; i++) {
      text_list[i].type = text[i].type;
      /* Prevent issues with TEXT.keyword[80] going out of scope */
      text_list[i].keyword = text_list[i].user_keyword_storage;
      memcpy(text_list[i].user_keyword_storage, text[i].keyword, strlen(text[i].keyword));
      text_list[i].text = text[i].text;
      text_list[i].text_length = text[i].length;

      if (text[i].type == PNG_ZTXT) {
        text_list[i].compression_flag = 1;
      } else if (text[i].type == PNG_ITXT) {
        text_list[i].compression_flag = text[i].compression_flag;
        text_list[i].language_tag = text[i].language_tag;
        text_list[i].translated_keyword = text[i].translated_keyword;
      }
    }

    _text_list = text_list;
    _n_text = n_text;

    _stored.text = 1;
    _user.text = 1;

    return 0;
  }

  int PngParser::set_bkgd(const struct BKGD &bkgd) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (!_stored.ihdr) return 1;

    if (_ihdr.color_type == 0 || _ihdr.color_type == 4) {
      _bkgd.gray = bkgd.gray;
    } else if (_ihdr.color_type == 2 || _ihdr.color_type == 6) {
      _bkgd.red = bkgd.red;
      _bkgd.green = bkgd.green;
      _bkgd.blue = bkgd.blue;
    } else if (_ihdr.color_type == 3) {
      if (!_stored.plte) return PNG_EBKGD_NO_PLTE;
      if (bkgd.plte_index >= _plte.n_entries) return PNG_EBKGD_PLTE_IDX;

      _bkgd.plte_index = bkgd.plte_index;
    }

    _stored.bkgd = 1;
    _user.bkgd = 1;

    return 0;
  }

  int PngParser::set_hist(const struct HIST &hist) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (!_stored.plte) return PNG_EHIST_NO_PLTE;

    _hist = hist;

    _stored.hist = 1;
    _user.hist = 1;

    return 0;
  }

  int PngParser::set_phys(const struct PHYS &phys) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (check_phys(phys)) return PNG_EPHYS;

    _phys = phys;

    _stored.phys = 1;
    _user.phys = 1;

    return 0;
  }

  int PngParser::set_splt(struct SPLT *splt, uint32_t n_splt) {
    if (!n_splt || !splt) return 1;
    PNG_SET_CHUNK_BOILERPLATE();

    uint32_t i;
    for (i = 0; i < n_splt; i++) {
      if (check_png_keyword(splt[i].name)) return PNG_ESPLT_NAME;
      if (!(splt[i].sample_depth == 8 || splt[i].sample_depth == 16)) return PNG_ESPLT_DEPTH;
    }

    if (_stored.splt && !_user.splt) {
      for (i = 0; i < _n_splt; i++) {
        if (_splt_list[i].entries != nullptr) free(_splt_list[i].entries);
      }
      free(_splt_list);
    }

    _splt_list = splt;
    _n_splt = n_splt;

    _stored.splt = 1;
    _user.splt = 1;

    return 0;
  }

  int PngParser::set_time(const struct Time &time) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (check_time(time)) return PNG_ETIME;

    _time = time;

    _stored.time = 1;
    _user.time = 1;

    return 0;
  }

  int PngParser::set_unknown_chunks(struct UnknownChunk *chunks, uint32_t n_chunks) {
    if (!n_chunks || !chunks) return 1;
    PNG_SET_CHUNK_BOILERPLATE();

    uint32_t i;
    for (i = 0; i < n_chunks; i++) {
      if (chunks[i].length > _u32max) return PNG_ECHUNK_STDLEN;
      if (chunks[i].length && chunks[i].data == nullptr) return 1;

      switch (chunks[i].location) {
        case PNG_AFTER_IHDR:
        case PNG_AFTER_PLTE:
        case PNG_AFTER_IDAT:
          break;
        default:
          return PNG_ECHUNK_POS;
      }
    }

    if (_stored.unknown && !_user.unknown) {
      for (i = 0; i < _n_chunks; i++) {
        free(_chunk_list[i].data);
      }
      free(_chunk_list);
    }

    _chunk_list = chunks;
    _n_chunks = n_chunks;

    _stored.unknown = 1;
    _user.unknown = 1;

    return 0;
  }

  int PngParser::set_offs(const struct Offs &offs) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (check_offs(offs)) return PNG_EOFFS;

    _offs = offs;

    _stored.offs = 1;
    _user.offs = 1;

    return 0;
  }

  int PngParser::set_exif(const struct Exif &exif) {
    PNG_SET_CHUNK_BOILERPLATE();

    if (check_exif(exif)) return PNG_EEXIF;

    if (_exif.data != nullptr && !_user.exif) free(_exif.data);

    _exif = exif;

    _stored.exif = 1;
    _user.exif = 1;

    return 0;
  }

  const char *PngParser::_strerror(int err) {
    switch (err) {
      case PNG_IO_EOF:
        return "end of stream";
      case PNG_IO_ERROR:
        return "stream error";
      case PNG_OK:
        return "success";
      case PNG_EINVAL:
        return "invalid argument";
      case PNG_EMEM:
        return "out of memory";
      case PNG_EOVERFLOW:
        return "arithmetic overflow";
      case PNG_ESIGNATURE:
        return "invalid signature";
      case PNG_EWIDTH:
        return "invalid image width";
      case PNG_EHEIGHT:
        return "invalid image height";
      case PNG_EUSER_WIDTH:
        return "image width exceeds user limit";
      case PNG_EUSER_HEIGHT:
        return "image height exceeds user limit";
      case PNG_EBIT_DEPTH:
        return "invalid bit depth";
      case PNG_ECOLOR_TYPE:
        return "invalid color type";
      case PNG_ECOMPRESSION_METHOD:
        return "invalid compression method";
      case PNG_EFILTER_METHOD:
        return "invalid filter method";
      case PNG_EINTERLACE_METHOD:
        return "invalid interlace method";
      case PNG_EIHDR_SIZE:
        return "invalid IHDR chunk size";
      case PNG_ENOIHDR:
        return "missing IHDR chunk";
      case PNG_ECHUNK_POS:
        return "invalid chunk position";
      case PNG_ECHUNK_SIZE:
        return "invalid chunk length";
      case PNG_ECHUNK_CRC:
        return "invalid chunk checksum";
      case PNG_ECHUNK_TYPE:
        return "invalid chunk type";
      case PNG_ECHUNK_UNKNOWN_CRITICAL:
        return "unknown critical chunk";
      case PNG_EDUP_PLTE:
        return "duplicate PLTE chunk";
      case PNG_EDUP_CHRM:
        return "duplicate cHRM chunk";
      case PNG_EDUP_GAMA:
        return "duplicate gAMA chunk";
      case PNG_EDUP_ICCP:
        return "duplicate iCCP chunk";
      case PNG_EDUP_SBIT:
        return "duplicate sBIT chunk";
      case PNG_EDUP_SRGB:
        return "duplicate sRGB chunk";
      case PNG_EDUP_BKGD:
        return "duplicate bKGD chunk";
      case PNG_EDUP_HIST:
        return "duplicate hIST chunk";
      case PNG_EDUP_TRNS:
        return "duplicate tRNS chunk";
      case PNG_EDUP_PHYS:
        return "duplicate pHYs chunk";
      case PNG_EDUP_TIME:
        return "duplicate tIME chunk";
      case PNG_EDUP_OFFS:
        return "duplicate oFFs chunk";
      case PNG_EDUP_EXIF:
        return "duplicate eXIf chunk";
      case PNG_ECHRM:
        return "invalid cHRM chunk";
      case PNG_EPLTE_IDX:
        return "invalid palette (PLTE) index";
      case PNG_ETRNS_COLOR_TYPE:
        return "tRNS chunk with incompatible color type";
      case PNG_ETRNS_NO_PLTE:
        return "missing palette (PLTE) for tRNS chunk";
      case PNG_EGAMA:
        return "invalid gAMA chunk";
      case PNG_EICCP_NAME:
        return "invalid iCCP profile name";
      case PNG_EICCP_COMPRESSION_METHOD:
        return "invalid iCCP compression method";
      case PNG_ESBIT:
        return "invalid sBIT chunk";
      case PNG_ESRGB:
        return "invalid sRGB chunk";
      case PNG_ETEXT:
        return "invalid tEXt chunk";
      case PNG_ETEXT_KEYWORD:
        return "invalid tEXt keyword";
      case PNG_EZTXT:
        return "invalid zTXt chunk";
      case PNG_EZTXT_COMPRESSION_METHOD:
        return "invalid zTXt compression method";
      case PNG_EITXT:
        return "invalid iTXt chunk";
      case PNG_EITXT_COMPRESSION_FLAG:
        return "invalid iTXt compression flag";
      case PNG_EITXT_COMPRESSION_METHOD:
        return "invalid iTXt compression method";
      case PNG_EITXT_LANG_TAG:
        return "invalid iTXt language tag";
      case PNG_EITXT_TRANSLATED_KEY:
        return "invalid iTXt translated key";
      case PNG_EBKGD_NO_PLTE:
        return "missing palette for bKGD chunk";
      case PNG_EBKGD_PLTE_IDX:
        return "invalid palette index for bKGD chunk";
      case PNG_EHIST_NO_PLTE:
        return "missing palette for hIST chunk";
      case PNG_EPHYS:
        return "invalid pHYs chunk";
      case PNG_ESPLT_NAME:
        return "invalid suggested palette name";
      case PNG_ESPLT_DUP_NAME:
        return "duplicate suggested palette (sPLT) name";
      case PNG_ESPLT_DEPTH:
        return "invalid suggested palette (sPLT) sample depth";
      case PNG_ETIME:
        return "invalid tIME chunk";
      case PNG_EOFFS:
        return "invalid oFFs chunk";
      case PNG_EEXIF:
        return "invalid eXIf chunk";
      case PNG_EIDAT_TOO_SHORT:
        return "IDAT stream too short";
      case PNG_EIDAT_STREAM:
        return "IDAT stream error";
      case PNG_EZLIB:
        return "zlib error";
      case PNG_EFILTER:
        return "invalid scanline filter";
      case PNG_EBUFSIZ:
        return "invalid buffer size";
      case PNG_EIO:
        return "i/o error";
      case PNG_EOF:
        return "end of file";
      case PNG_EBUF_SET:
        return "buffer already set";
      case PNG_EBADSTATE:
        return "non-recoverable state";
      case PNG_EFMT:
        return "invalid format";
      case PNG_EFLAGS:
        return "invalid flags";
      case PNG_ECHUNKAVAIL:
        return "chunk not available";
      case PNG_ENCODE_ONLY:
        return "encode only context";
      case PNG_EOI:
        return "reached end-of-image state";
      case PNG_ENOPLTE:
        return "missing PLTE for indexed image";
      case PNG_ECHUNK_LIMITS:
        return "reached chunk/cache limits";
      case PNG_EZLIB_INIT:
        return "zlib init error";
      case PNG_ECHUNK_STDLEN:
        return "chunk exceeds maximum standard length";
      case PNG_EINTERNAL:
        return "internal error";
      case PNG_ECTXTYPE:
        return "invalid operation for context type";
      case PNG_ENOSRC:
        return "source PNG not set";
      case PNG_ENODST:
        return "PNG output not set";
      case PNG_EOPSTATE:
        return "invalid operation for state";
      case PNG_ENOTFINAL:
        return "PNG not finalized";
      default:
        return "unknown error";
    }
  }

  const char *versionStr() {
    return "0.7.3";
  }

#pragma GCC target("sse2")

#include <immintrin.h>

/* Functions in this file look at most 3 pixels (a,b,c) to predict the 4th (d).
 * They're positioned like this:
 *    prev:  c b
 *    row:   a d
 * The Sub filter predicts d=a, Avg d=(a+b)/2, and Paeth predicts d to be
 * whichever of a, b, or c is closest to p=a+b-c.
 */

  static __m128i load4(const void *p) {
    int tmp;
    memcpy(&tmp, p, sizeof(tmp));
    return _mm_cvtsi32_si128(tmp);
  }

  static void store4(void *p, __m128i v) {
    int tmp = _mm_cvtsi128_si32(v);
    memcpy(p, &tmp, sizeof(int));
  }

  static __m128i load3(const void *p) {
    uint32_t tmp = 0;
    memcpy(&tmp, p, 3);
    return _mm_cvtsi32_si128(tmp);
  }

  static void store3(void *p, __m128i v) {
    int tmp = _mm_cvtsi128_si32(v);
    memcpy(p, &tmp, 3);
  }

  static void defilter_sub3(size_t rowbytes, unsigned char *row) {
    /* The Sub filter predicts each pixel as the previous pixel, a.
     * There is no pixel to the left of the first pixel.  It's encoded directly.
     * That works with our main loop if we just say that left pixel was zero.
     */
    size_t rb = rowbytes;

    __m128i a, d = _mm_setzero_si128();

    while (rb >= 4) {
      a = d;
      d = load4(row);
      d = _mm_add_epi8(d, a);
      store3(row, d);

      row += 3;
      rb -= 3;
    }

    if (rb > 0) {
      a = d;
      d = load3(row);
      d = _mm_add_epi8(d, a);
      store3(row, d);
    }
  }

  static void defilter_sub4(size_t rowbytes, unsigned char *row) {
    /* The Sub filter predicts each pixel as the previous pixel, a.
     * There is no pixel to the left of the first pixel.  It's encoded directly.
     * That works with our main loop if we just say that left pixel was zero.
     */
    size_t rb = rowbytes + 4;

    __m128i a, d = _mm_setzero_si128();

    while (rb > 4) {
      a = d;
      d = load4(row);
      d = _mm_add_epi8(d, a);
      store4(row, d);

      row += 4;
      rb -= 4;
    }
  }

  static void defilter_avg3(size_t rowbytes, unsigned char *row, const unsigned char *prev) {
    /* The Avg filter predicts each pixel as the (truncated) average of a and b.
     * There's no pixel to the left of the first pixel.  Luckily, it's
     * predicted to be half of the pixel above it.  So again, this works
     * perfectly with our loop if we make sure a starts at zero.
     */

    size_t rb = rowbytes;

    const __m128i zero = _mm_setzero_si128();

    __m128i b;
    __m128i a, d = zero;

    while (rb >= 4) {
      __m128i avg;
      b = load4(prev);
      a = d;
      d = load4(row);

      /* PNG requires a truncating average, so we can't just use _mm_avg_epu8 */
      avg = _mm_avg_epu8(a, b);
      /* ...but we can fix it up by subtracting off 1 if it rounded up. */
      avg = _mm_sub_epi8(avg, _mm_and_si128(_mm_xor_si128(a, b),
                                            _mm_set1_epi8(1)));
      d = _mm_add_epi8(d, avg);
      store3(row, d);

      prev += 3;
      row += 3;
      rb -= 3;
    }

    if (rb > 0) {
      __m128i avg;
      b = load3(prev);
      a = d;
      d = load3(row);

      /* PNG requires a truncating average, so we can't just use _mm_avg_epu8 */
      avg = _mm_avg_epu8(a, b);
      /* ...but we can fix it up by subtracting off 1 if it rounded up. */
      avg = _mm_sub_epi8(avg, _mm_and_si128(_mm_xor_si128(a, b),
                                            _mm_set1_epi8(1)));

      d = _mm_add_epi8(d, avg);
      store3(row, d);
    }
  }

  static void defilter_avg4(size_t rowbytes, unsigned char *row, const unsigned char *prev) {
    /* The Avg filter predicts each pixel as the (truncated) average of a and b.
     * There's no pixel to the left of the first pixel.  Luckily, it's
     * predicted to be half of the pixel above it.  So again, this works
     * perfectly with our loop if we make sure a starts at zero.
     */
    size_t rb = rowbytes + 4;

    const __m128i zero = _mm_setzero_si128();
    __m128i b;
    __m128i a, d = zero;

    while (rb > 4) {
      __m128i avg;
      b = load4(prev);
      a = d;
      d = load4(row);

      /* PNG requires a truncating average, so we can't just use _mm_avg_epu8 */
      avg = _mm_avg_epu8(a, b);
      /* ...but we can fix it up by subtracting off 1 if it rounded up. */
      avg = _mm_sub_epi8(avg, _mm_and_si128(_mm_xor_si128(a, b),
                                            _mm_set1_epi8(1)));

      d = _mm_add_epi8(d, avg);
      store4(row, d);

      prev += 4;
      row += 4;
      rb -= 4;
    }
  }

  static __m128i abs_i16(__m128i x) {
    /* Read this all as, return x<0 ? -x : x.
   * To negate two's complement, you flip all the bits then add 1.
   */
    __m128i is_negative = _mm_cmplt_epi16(x, _mm_setzero_si128());

    /* Flip negative lanes. */
    x = _mm_xor_si128(x, is_negative);

    /* +1 to negative lanes, else +0. */
    x = _mm_sub_epi16(x, is_negative);
    return x;
  }

/* Bytewise c ? t : e. */
  static __m128i if_then_else(__m128i c, __m128i t, __m128i e) {
    return _mm_or_si128(_mm_and_si128(c, t), _mm_andnot_si128(c, e));
  }

  static void defilter_paeth3(size_t rowbytes, unsigned char *row, const unsigned char *prev) {
    /* Paeth tries to predict pixel d using the pixel to the left of it, a,
     * and two pixels from the previous row, b and c:
     *   prev: c b
     *   row:  a d
     * The Paeth function predicts d to be whichever of a, b, or c is nearest to
     * p=a+b-c.
     *
     * The first pixel has no left context, and so uses an Up filter, p = b.
     * This works naturally with our main loop's p = a+b-c if we force a and c
     * to zero.
     * Here we zero b and d, which become c and a respectively at the start of
     * the loop.
     */
    size_t rb = rowbytes;
    const __m128i zero = _mm_setzero_si128();
    __m128i c, b = zero,
        a, d = zero;

    while (rb >= 4) {
      /* It's easiest to do this math (particularly, deal with pc) with 16-bit
         * intermediates.
         */
      __m128i pa, pb, pc, smallest, nearest;
      c = b;
      b = _mm_unpacklo_epi8(load4(prev), zero);
      a = d;
      d = _mm_unpacklo_epi8(load4(row), zero);

      /* (p-a) == (a+b-c - a) == (b-c) */

      pa = _mm_sub_epi16(b, c);

      /* (p-b) == (a+b-c - b) == (a-c) */
      pb = _mm_sub_epi16(a, c);

      /* (p-c) == (a+b-c - c) == (a+b-c-c) == (b-c)+(a-c) */
      pc = _mm_add_epi16(pa, pb);

      pa = abs_i16(pa);  /* |p-a| */
      pb = abs_i16(pb);  /* |p-b| */
      pc = abs_i16(pc);  /* |p-c| */

      smallest = _mm_min_epi16(pc, _mm_min_epi16(pa, pb));

      /* Paeth breaks ties favoring a over b over c. */
      nearest = if_then_else(_mm_cmpeq_epi16(smallest, pa), a,
                             if_then_else(_mm_cmpeq_epi16(smallest, pb), b, c));

      /* Note `_epi8`: we need addition to wrap modulo 255. */
      d = _mm_add_epi8(d, nearest);
      store3(row, _mm_packus_epi16(d, d));

      prev += 3;
      row += 3;
      rb -= 3;
    }

    if (rb > 0) {
      /* It's easiest to do this math (particularly, deal with pc) with 16-bit
         * intermediates.
         */
      __m128i pa, pb, pc, smallest, nearest;
      c = b;
      b = _mm_unpacklo_epi8(load3(prev), zero);
      a = d;
      d = _mm_unpacklo_epi8(load3(row), zero);

      /* (p-a) == (a+b-c - a) == (b-c) */
      pa = _mm_sub_epi16(b, c);

      /* (p-b) == (a+b-c - b) == (a-c) */
      pb = _mm_sub_epi16(a, c);

      /* (p-c) == (a+b-c - c) == (a+b-c-c) == (b-c)+(a-c) */
      pc = _mm_add_epi16(pa, pb);

      pa = abs_i16(pa);  /* |p-a| */
      pb = abs_i16(pb);  /* |p-b| */
      pc = abs_i16(pc);  /* |p-c| */

      smallest = _mm_min_epi16(pc, _mm_min_epi16(pa, pb));

      /* Paeth breaks ties favoring a over b over c. */
      nearest = if_then_else(_mm_cmpeq_epi16(smallest, pa), a,
                             if_then_else(_mm_cmpeq_epi16(smallest, pb), b, c));

      /* Note `_epi8`: we need addition to wrap modulo 255. */
      d = _mm_add_epi8(d, nearest);
      store3(row, _mm_packus_epi16(d, d));
    }
  }

  static void defilter_paeth4(size_t rowbytes, unsigned char *row, const unsigned char *prev) {
    /* Paeth tries to predict pixel d using the pixel to the left of it, a,
     * and two pixels from the previous row, b and c:
     *   prev: c b
     *   row:  a d
     * The Paeth function predicts d to be whichever of a, b, or c is nearest to
     * p=a+b-c.
     *
     * The first pixel has no left context, and so uses an Up filter, p = b.
     * This works naturally with our main loop's p = a+b-c if we force a and c
     * to zero.
     * Here we zero b and d, which become c and a respectively at the start of
     * the loop.
     */
    size_t rb = rowbytes + 4;

    const __m128i zero = _mm_setzero_si128();
    __m128i pa, pb, pc, smallest, nearest;
    __m128i c, b = zero,
        a, d = zero;

    while (rb > 4) {
      /* It's easiest to do this math (particularly, deal with pc) with 16-bit
         * intermediates.
         */
      c = b;
      b = _mm_unpacklo_epi8(load4(prev), zero);
      a = d;
      d = _mm_unpacklo_epi8(load4(row), zero);

      /* (p-a) == (a+b-c - a) == (b-c) */
      pa = _mm_sub_epi16(b, c);

      /* (p-b) == (a+b-c - b) == (a-c) */
      pb = _mm_sub_epi16(a, c);

      /* (p-c) == (a+b-c - c) == (a+b-c-c) == (b-c)+(a-c) */
      pc = _mm_add_epi16(pa, pb);

      pa = abs_i16(pa);  /* |p-a| */
      pb = abs_i16(pb);  /* |p-b| */
      pc = abs_i16(pc);  /* |p-c| */

      smallest = _mm_min_epi16(pc, _mm_min_epi16(pa, pb));

      /* Paeth breaks ties favoring a over b over c. */
      nearest = if_then_else(_mm_cmpeq_epi16(smallest, pa), a,
                             if_then_else(_mm_cmpeq_epi16(smallest, pb), b, c));

      /* Note `_epi8`: we need addition to wrap modulo 255. */
      d = _mm_add_epi8(d, nearest);
      store4(row, _mm_packus_epi16(d, d));

      prev += 4;
      row += 4;
      rb -= 4;
    }
  }
}