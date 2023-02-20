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
#include <stdint.h>
#include <stdio.h>
#include <zlib.h>

namespace upanui {
  namespace PngEncoder {

    enum ErrCode {
      PNG_IO_ERROR = -2,
      PNG_IO_EOF = -1,
      PNG_OK = 0,
      PNG_EINVAL,
      PNG_EMEM,
      PNG_EOVERFLOW,
      PNG_ESIGNATURE,
      PNG_EWIDTH,
      PNG_EHEIGHT,
      PNG_EUSER_WIDTH,
      PNG_EUSER_HEIGHT,
      PNG_EBIT_DEPTH,
      PNG_ECOLOR_TYPE,
      PNG_ECOMPRESSION_METHOD,
      PNG_EFILTER_METHOD,
      PNG_EINTERLACE_METHOD,
      PNG_EIHDR_SIZE,
      PNG_ENOIHDR,
      PNG_ECHUNK_POS,
      PNG_ECHUNK_SIZE,
      PNG_ECHUNK_CRC,
      PNG_ECHUNK_TYPE,
      PNG_ECHUNK_UNKNOWN_CRITICAL,
      PNG_EDUP_PLTE,
      PNG_EDUP_CHRM,
      PNG_EDUP_GAMA,
      PNG_EDUP_ICCP,
      PNG_EDUP_SBIT,
      PNG_EDUP_SRGB,
      PNG_EDUP_BKGD,
      PNG_EDUP_HIST,
      PNG_EDUP_TRNS,
      PNG_EDUP_PHYS,
      PNG_EDUP_TIME,
      PNG_EDUP_OFFS,
      PNG_EDUP_EXIF,
      PNG_ECHRM,
      PNG_EPLTE_IDX,
      PNG_ETRNS_COLOR_TYPE,
      PNG_ETRNS_NO_PLTE,
      PNG_EGAMA,
      PNG_EICCP_NAME,
      PNG_EICCP_COMPRESSION_METHOD,
      PNG_ESBIT,
      PNG_ESRGB,
      PNG_ETEXT,
      PNG_ETEXT_KEYWORD,
      PNG_EZTXT,
      PNG_EZTXT_COMPRESSION_METHOD,
      PNG_EITXT,
      PNG_EITXT_COMPRESSION_FLAG,
      PNG_EITXT_COMPRESSION_METHOD,
      PNG_EITXT_LANG_TAG,
      PNG_EITXT_TRANSLATED_KEY,
      PNG_EBKGD_NO_PLTE,
      PNG_EBKGD_PLTE_IDX,
      PNG_EHIST_NO_PLTE,
      PNG_EPHYS,
      PNG_ESPLT_NAME,
      PNG_ESPLT_DUP_NAME,
      PNG_ESPLT_DEPTH,
      PNG_ETIME,
      PNG_EOFFS,
      PNG_EEXIF,
      PNG_EIDAT_TOO_SHORT,
      PNG_EIDAT_STREAM,
      PNG_EZLIB,
      PNG_EFILTER,
      PNG_EBUFSIZ,
      PNG_EIO,
      PNG_EOF,
      PNG_EBUF_SET,
      PNG_EBADSTATE,
      PNG_EFMT,
      PNG_EFLAGS,
      PNG_ECHUNKAVAIL,
      PNG_ENCODE_ONLY,
      PNG_EOI,
      PNG_ENOPLTE,
      PNG_ECHUNK_LIMITS,
      PNG_EZLIB_INIT,
      PNG_ECHUNK_STDLEN,
      PNG_EINTERNAL,
      PNG_ECTXTYPE,
      PNG_ENOSRC,
      PNG_ENODST,
      PNG_EOPSTATE,
      PNG_ENOTFINAL,
    };

    enum TextType {
      PNG_TEXT = 1,
      PNG_ZTXT = 2,
      PNG_ITXT = 3
    };

    enum ColorType {
      PNG_COLOR_TYPE_GRAYSCALE = 0,
      PNG_COLOR_TYPE_TRUECOLOR = 2,
      PNG_COLOR_TYPE_INDEXED = 3,
      PNG_COLOR_TYPE_GRAYSCALE_ALPHA = 4,
      PNG_COLOR_TYPE_TRUECOLOR_ALPHA = 6
    };

    enum Filter {
      PNG_FILTER_NONE = 0,
      PNG_FILTER_SUB = 1,
      PNG_FILTER_UP = 2,
      PNG_FILTER_AVERAGE = 3,
      PNG_FILTER_PAETH = 4
    };

    enum FilterChoice {
      PNG_DISABLE_FILTERING = 0,
      PNG_FILTER_CHOICE_NONE = 8,
      PNG_FILTER_CHOICE_SUB = 16,
      PNG_FILTER_CHOICE_UP = 32,
      PNG_FILTER_CHOICE_AVG = 64,
      PNG_FILTER_CHOICE_PAETH = 128,
      PNG_FILTER_CHOICE_ALL = (8 | 16 | 32 | 64 | 128)
    };

    enum InterlaceMethod {
      PNG_INTERLACE_NONE = 0,
      PNG_INTERLACE_ADAM7 = 1
    };

    // Channels are always in byte-order
    enum Format {
      PNG_FMT_RGBA8 = 1,
      PNG_FMT_RGBA16 = 2,
      PNG_FMT_RGB8 = 4,

      // No conversion or scaling
      PNG_FMT_PNG = 256,
      PNG_FMT_RAW = 512  // big-endian (everything else is host-endian)
    };

    enum ContextFlags {
      PNG_CTX_IGNORE_ADLER32 = 1, /* Ignore checksum in DEFLATE streams */
      PNG_CTX_ENCODER = 2 /* Create an encoder context */
    };

    enum DecodeFlags {
      PNG_DECODE_USE_SBIT = 8, /* Undocumented */

      PNG_DECODE_TRNS = 1, /* Apply transparency */
      PNG_DECODE_GAMMA = 2, /* Apply gamma correction */
      PNG_DECODE_PROGRESSIVE = 256 /* Initialize for progressive reads */
    };

    enum CRCAction {
      /* Default for critical chunks */
      PNG_CRC_ERROR = 0,

      /* Discard chunk, invalid for critical chunks.
         Since v0.6.2: default for ancillary chunks */
      PNG_CRC_DISCARD = 1,

      /* Ignore and don't calculate checksum.
         Since v0.6.2: also ignores checksums in DEFLATE streams */
      PNG_CRC_USE = 2
    };

    enum EncodeFlags {
      PNG_ENCODE_PROGRESSIVE = 1, /* Initialize for progressive writes */
      PNG_ENCODE_FINALIZE = 2, /* Finalize PNG after encoding image */
    };

    struct iHeader {
      uint32_t width;
      uint32_t height;
      uint8_t bit_depth;
      uint8_t color_type;
      uint8_t compression_method;
      uint8_t filter_method;
      uint8_t interlace_method;
    };

    struct PLTEEntry {
      uint8_t red;
      uint8_t green;
      uint8_t blue;

      uint8_t alpha; /* Reserved for internal use */
    };

    struct PLTE {
      uint32_t n_entries;
      struct PLTEEntry entries[256];
    };

    struct Trns {
      uint16_t gray;

      uint16_t red;
      uint16_t green;
      uint16_t blue;

      uint32_t n_type3_entries;
      uint8_t type3_alpha[256];
    };

    struct ChrmInt {
      uint32_t white_point_x;
      uint32_t white_point_y;
      uint32_t red_x;
      uint32_t red_y;
      uint32_t green_x;
      uint32_t green_y;
      uint32_t blue_x;
      uint32_t blue_y;
    };

    struct Chrm {
      double white_point_x;
      double white_point_y;
      double red_x;
      double red_y;
      double green_x;
      double green_y;
      double blue_x;
      double blue_y;
    };

    struct ICCP {
      char profile_name[80];
      size_t profile_len;
      char *profile;
    };

    struct SBIT {
      uint8_t grayscale_bits;
      uint8_t red_bits;
      uint8_t green_bits;
      uint8_t blue_bits;
      uint8_t alpha_bits;
    };

    struct TEXT {
      char keyword[80];
      int type;

      size_t length;
      char *text;

      uint8_t compression_flag; /* iTXt only */
      uint8_t compression_method; /* iTXt, ztXt only */
      char *language_tag; /* iTXt only */
      char *translated_keyword; /* iTXt only */
    };

    struct BKGD {
      uint16_t gray; /* Only for gray/gray alpha */
      uint16_t red;
      uint16_t green;
      uint16_t blue;
      uint16_t plte_index; /* Only for indexed color */
    };

    struct HIST {
      uint16_t frequency[256];
    };

    struct PHYS {
      uint32_t ppu_x, ppu_y;
      uint8_t unit_specifier;
    };

    struct SPLTEntry {
      uint16_t red;
      uint16_t green;
      uint16_t blue;
      uint16_t alpha;
      uint16_t frequency;
    };

    struct SPLT {
      char name[80];
      uint8_t sample_depth;
      uint32_t n_entries;
      struct SPLTEntry *entries;
    };

    struct Time {
      uint16_t year;
      uint8_t month;
      uint8_t day;
      uint8_t hour;
      uint8_t minute;
      uint8_t second;
    };

    struct Offs {
      int32_t x, y;
      uint8_t unit_specifier;
    };

    struct Exif {
      size_t length;
      char *data;
    };

    struct Chunk {
      size_t offset;
      uint32_t length;
      uint8_t type[4];
      uint32_t crc;
    };

    enum Location {
      PNG_AFTER_IHDR = 1,
      PNG_AFTER_PLTE = 2,
      PNG_AFTER_IDAT = 8,
    };

    struct UnknownChunk {
      uint8_t type[4];
      size_t length;
      void *data;
      enum Location location;
    };

    enum Option {
      PNG_KEEP_UNKNOWN_CHUNKS = 1,

      PNG_IMG_COMPRESSION_LEVEL,
      PNG_IMG_WINDOW_BITS,
      PNG_IMG_MEM_LEVEL,
      PNG_IMG_COMPRESSION_STRATEGY,

      PNG_TEXT_COMPRESSION_LEVEL,
      PNG_TEXT_WINDOW_BITS,
      PNG_TEXT_MEM_LEVEL,
      PNG_TEXT_COMPRESSION_STRATEGY,

      PNG_FILTER_CHOICE,
      PNG_CHUNK_COUNT_LIMIT,
      PNG_ENCODE_TO_BUFFER,
    };

    enum State {
      PNG_STATE_INVALID = 0,
      PNG_STATE_INIT = 1, /* No PNG buffer/stream is set */
      PNG_STATE_INPUT, /* Decoder input PNG was set */
      PNG_STATE_OUTPUT = PNG_STATE_INPUT, /* Encoder output was set */
      PNG_STATE_IHDR, /* IHDR was read/written */
      PNG_STATE_FIRST_IDAT,  /* Encoded up to / reached first IDAT */
      PNG_STATE_DECODE_INIT, /* Decoder is ready for progressive reads */
      PNG_STATE_ENCODE_INIT = PNG_STATE_DECODE_INIT,
      PNG_STATE_EOI, /* Reached the last scanline/row */
      PNG_STATE_LAST_IDAT, /* Reached last IDAT, set at end of decode_image() */
      PNG_STATE_AFTER_IDAT, /*  */
      PNG_STATE_IEND, /* Reached IEND */
    };

    enum _Internal {
      PNG__IO_SIGNAL = 1 << 9,
      PNG__CTX_FLAGS_ALL = (PNG_CTX_IGNORE_ADLER32 | PNG_CTX_ENCODER)
    };

    struct RowInfo {
      uint32_t scanline_idx;
      uint32_t row_num; /* deinterlaced row index */
      int pass;
      uint8_t filter;
    };

    class Context;

    typedef int spng_read_fn(Context *ctx, void *user, void *dest, size_t length);

    typedef int spng_write_fn(Context *ctx, void *user, void *src, size_t length);

    typedef int spng_rw_fn(Context *ctx, void *user, void *dst_src, size_t length);

    typedef void spng__undo(Context *ctx);


    struct spng_subimage {
      uint32_t width;
      uint32_t height;
      size_t out_width; /* byte width based on output format */
      size_t scanline_width;
    };

    struct spng_text2 {
      int type;
      char *keyword;
      char *text;

      size_t text_length;

      uint8_t compression_flag; /* iTXt only */
      char *language_tag; /* iTXt only */
      char *translated_keyword; /* iTXt only */

      size_t cache_usage;
      char user_keyword_storage[80];
    };

    struct decode_flags {
      unsigned apply_trns: 1;
      unsigned apply_gamma: 1;
      unsigned use_sbit: 1;
      unsigned indexed: 1;
      unsigned do_scaling: 1;
      unsigned interlaced: 1;
      unsigned same_layout: 1;
      unsigned zerocopy: 1;
      unsigned unpack: 1;
    };

    struct encode_flags {
      unsigned interlace: 1;
      unsigned same_layout: 1;
      unsigned to_bigendian: 1;
      unsigned progressive: 1;
      unsigned finalize: 1;

      FilterChoice filter_choice;
    };

    struct spng_chunk_bitfield {
      unsigned ihdr: 1;
      unsigned plte: 1;
      unsigned chrm: 1;
      unsigned iccp: 1;
      unsigned gama: 1;
      unsigned sbit: 1;
      unsigned srgb: 1;
      unsigned text: 1;
      unsigned bkgd: 1;
      unsigned hist: 1;
      unsigned trns: 1;
      unsigned phys: 1;
      unsigned splt: 1;
      unsigned time: 1;
      unsigned offs: 1;
      unsigned exif: 1;
      unsigned unknown: 1;
    };

/* Packed sample iterator */
    struct spng__iter {
      const uint8_t mask;
      unsigned shift_amount;
      const unsigned initial_shift, bit_depth;
      const unsigned char *samples;
    };

    union spng__decode_plte {
      struct PLTEEntry rgba[256];
      unsigned char rgb[256 * 3];
      unsigned char raw[256 * 4];
      uint32_t align_this;
    };

    struct spng__zlib_options {
      int compression_level;
      int window_bits;
      int mem_level;
      int strategy;
      int data_type;
    };

    class Context {
    public:
      Context(int flags);
      ~Context();

      int read_chunks(bool only_ihdr);
      int read_data(size_t bytes);
      int read_ihdr();
      int read_and_check_crc();
      int read_header();
      int read_chunk_bytes(uint32_t bytes);
      int discard_chunk_bytes(uint32_t bytes);
      int read_idat_bytes(uint32_t *bytes_read);
      int read_scanline();
      int read_scanline_bytes(unsigned char *dest, size_t len);
      int read_non_idat_chunks();
      int spng_decode_image(void *out, size_t len, int fmt, int flags);
      int spng_decode_chunks();
      int spng_decoded_image_size(int fmt, size_t *len);
      int spng_decode_scanline(void *out, size_t len);
      int spng_decode_row(void *out, size_t len);
      int on_error(int err);
      int require_bytes(size_t bytes);
      int write_data(const void *data, size_t bytes);
      int write_header(const uint8_t chunk_type[4], size_t chunk_length, unsigned char **data);
      int finish_chunk();
      int write_chunk(const uint8_t type[4], const void *data, size_t length);
      int write_iend();
      int write_unknown_chunks(Location location);
      int write_chunks_before_idat();
      int write_chunks_after_idat();
      int write_idat_bytes(const void *scanline, size_t len, int flush);
      int finish_idat();
      int trim_chunk(uint32_t length);
      int encode_scanline(const void *scanline, size_t len);
      int encode_row(const void *row, size_t len);
      int spng_encode_scanline(const void *scanline, size_t len);
      int spng_encode_row(const void *row, size_t len);
      int spng_encode_chunks();
      int spng_encode_image(const void *img, size_t len, int fmt, int flags);
      int update_row_info();
      int calculate_subimages();

      size_t _data_size;
      size_t _bytes_read;
      size_t _stream_buf_size;
      unsigned char *_stream_buf;
      const unsigned char *_data;

      /* User-defined pointers for streaming */
      spng_read_fn *_read_fn;
      spng_write_fn *_write_fn;
      void *_stream_user_ptr;

      /* Used for buffer reads */
      const unsigned char *_png_base;
      size_t _bytes_left;
      size_t _last_read_size;

      /* Used for encoding */
      int _user_owns_out_png;
      unsigned char *_out_png;
      unsigned char *_write_ptr;
      size_t _out_png_size;
      size_t _bytes_encoded;

      /* These are updated by read/write_header()/read_chunk_bytes() */
      struct Chunk _current_chunk;
      uint32_t _cur_chunk_bytes_left;
      uint32_t _cur_actual_crc;

      enum ContextFlags _flags;
      enum Format _fmt;

      enum State _state;

      unsigned _streaming: 1;
      unsigned _internal_buffer: 1; /* encoding to internal buffer */

      unsigned _inflate: 1;
      unsigned _deflate: 1;
      unsigned _encode_only: 1;
      unsigned _strict: 1;
      unsigned _discard: 1;
      unsigned _skip_crc: 1;
      unsigned _keep_unknown: 1;
      unsigned _prev_was_idat: 1;

      struct spng__zlib_options _image_options;
      struct spng__zlib_options _text_options;

      spng__undo *_undo;

      /* input file contains this chunk */
      struct spng_chunk_bitfield _file;

      /* chunk was stored with spng_set_*() */
      struct spng_chunk_bitfield _user;

      /* chunk was stored by reading or with spng_set_*() */
      struct spng_chunk_bitfield _stored;

      /* used to reset the above in case of an error */
      struct spng_chunk_bitfield _prev_stored;

      struct Chunk _first_idat, _last_idat;

      uint32_t _max_width, _max_height;

      size_t _max_chunk_size;
      size_t _chunk_cache_limit;
      size_t _chunk_cache_usage;
      uint32_t _chunk_count_limit;
      uint32_t _chunk_count_total;

      int _crc_action_critical;
      int _crc_action_ancillary;

      uint32_t _optimize_option;

      struct iHeader _ihdr;

      struct PLTE _plte;

      struct ChrmInt _chrm_int;
      struct ICCP _iccp;

      uint32_t _gama;

      struct SBIT _sbit;

      uint8_t _srgb_rendering_intent;

      uint32_t _n_text;
      struct spng_text2 *_text_list;

      struct BKGD _bkgd;
      struct HIST _hist;
      struct Trns _trns;
      struct PHYS _phys;

      uint32_t _n_splt;
      struct SPLT *_splt_list;

      struct Time _time;
      struct Offs _offs;
      struct Exif _exif;

      uint32_t _n_chunks;
      struct UnknownChunk *_chunk_list;

      struct spng_subimage _subimage[7];

      gccucore::zlib::z_stream _zstream;
      unsigned char *_scanline_buf, *_prev_scanline_buf, *_row_buf, *_filtered_scanline_buf;
      unsigned char *_scanline, *_prev_scanline, *_row, *_filtered_scanline;

      /* based on fmt */
      size_t _image_size; /* may be zero */
      size_t _image_width;

      unsigned _bytes_per_pixel; /* derived from ihdr */
      unsigned _pixel_size; /* derived from spng_format+ihdr */
      int _widest_pass;
      int _last_pass; /* last non-empty pass */

      uint16_t *_gamma_lut; /* points to either _lut8 or _lut16 */
      uint16_t *_gamma_lut16;
      uint16_t _gamma_lut8[256];
      unsigned char _trns_px[8];
      union spng__decode_plte _decode_plte;
      struct SBIT _decode_sb;
      struct decode_flags _decode_flags;
      struct RowInfo _row_info;

      struct encode_flags _encode_flags;
    };

    int spng_set_png_buffer(Context *ctx, const void *buf, size_t size);

    int spng_set_png_stream(Context *ctx, spng_rw_fn *rw_func, void *user);

    int spng_set_png_file(Context *ctx, FILE *file);

    void *spng_get_png_buffer(Context *ctx, size_t *len, int *error);

    int spng_set_image_limits(Context *ctx, uint32_t width, uint32_t height);

    int spng_get_image_limits(Context *ctx, uint32_t *width, uint32_t *height);

    int spng_set_chunk_limits(Context *ctx, size_t chunk_size, size_t cache_size);

    int spng_get_chunk_limits(Context *ctx, size_t *chunk_size, size_t *cache_size);

    int spng_set_crc_action(Context *ctx, int critical, int ancillary);

    int spng_set_option(Context *ctx, enum Option option, int value);

    int spng_get_option(Context *ctx, enum Option option, int *value);

/* Encode/decode */
    int spng_get_row_info(Context *ctx, struct RowInfo *row_info);

/* Encode */
    int spng_encode_image(Context *ctx, const void *img, size_t len, int fmt, int flags);

/* Progressive encode */
    int spng_encode_scanline(Context *ctx, const void *scanline, size_t len);

    int spng_encode_row(Context *ctx, const void *row, size_t len);

    int spng_encode_chunks(Context *ctx);

    int spng_get_ihdr(Context *ctx, struct iHeader *ihdr);

    int spng_get_plte(Context *ctx, struct PLTE *plte);

    int spng_get_trns(Context *ctx, struct Trns *trns);

    int spng_get_chrm(Context *ctx, struct Chrm *chrm);

    int spng_get_chrm_int(Context *ctx, struct ChrmInt *chrm_int);

    int spng_get_gama(Context *ctx, double *gamma);

    int spng_get_gama_int(Context *ctx, uint32_t *gama_int);

    int spng_get_iccp(Context *ctx, struct ICCP *iccp);

    int spng_get_sbit(Context *ctx, struct SBIT *sbit);

    int spng_get_srgb(Context *ctx, uint8_t *rendering_intent);

    int spng_get_text(Context *ctx, struct TEXT *text, uint32_t *n_text);

    int spng_get_bkgd(Context *ctx, struct BKGD *bkgd);

    int spng_get_hist(Context *ctx, struct HIST *hist);

    int spng_get_phys(Context *ctx, struct PHYS *phys);

    int spng_get_splt(Context *ctx, struct SPLT *splt, uint32_t *n_splt);

    int spng_get_time(Context *ctx, struct Time *time);

    int spng_get_unknown_chunks(Context *ctx, struct UnknownChunk *chunks, uint32_t *n_chunks);

/* Official extensions */
    int spng_get_offs(Context *ctx, struct Offs *offs);

    int spng_get_exif(Context *ctx, struct Exif *exif);


    int spng_set_ihdr(Context *ctx, struct iHeader *ihdr);

    int spng_set_plte(Context *ctx, struct PLTE *plte);

    int spng_set_trns(Context *ctx, struct Trns *trns);

    int spng_set_chrm(Context *ctx, struct Chrm *chrm);

    int spng_set_chrm_int(Context *ctx, struct ChrmInt *chrm_int);

    int spng_set_gama(Context *ctx, double gamma);

    int spng_set_gama_int(Context *ctx, uint32_t gamma);

    int spng_set_iccp(Context *ctx, struct ICCP *iccp);

    int spng_set_sbit(Context *ctx, struct SBIT *sbit);

    int spng_set_srgb(Context *ctx, uint8_t rendering_intent);

    int spng_set_text(Context *ctx, struct TEXT *text, uint32_t n_text);

    int spng_set_bkgd(Context *ctx, struct BKGD *bkgd);

    int spng_set_hist(Context *ctx, struct HIST *hist);

    int spng_set_phys(Context *ctx, struct PHYS *phys);

    int spng_set_splt(Context *ctx, struct SPLT *splt, uint32_t n_splt);

    int spng_set_time(Context *ctx, struct Time *time);

    int spng_set_unknown_chunks(Context *ctx, struct UnknownChunk *chunks, uint32_t n_chunks);

/* Official extensions */
    int spng_set_offs(Context *ctx, struct Offs *offs);

    int spng_set_exif(Context *ctx, struct Exif *exif);


    const char *spng_strerror(int err);

    const char *spng_version_string(void);
  }
}