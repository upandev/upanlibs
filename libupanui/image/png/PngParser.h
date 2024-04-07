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
#include <vector.h>

namespace upanui {
  class PngParser;

  typedef int _read_fn_ptr(PngParser *ctx, void *user, void *dest, size_t length);

  typedef int _write_fn_ptr(PngParser *ctx, void *user, void *src, size_t length);

  typedef int _rw_fn_ptr(PngParser *ctx, void *user, void *dst_src, size_t length);

  typedef void (PngParser::*undo_func_ptr)();

  class PngParser {
  public:
    PngParser(const void *buf, size_t size);
    ~PngParser();

    // Channels are always in byte-order
    enum Format {
      PNG_FMT_RGBA8 = 1,
      PNG_FMT_RGBA16 = 2,
      PNG_FMT_RGB8 = 4,

      // No conversion or scaling
      PNG_FMT_PNG = 256,
      PNG_FMT_RAW = 512  // big-endian (everything else is host-endian)
    };

    int decode_image(void *out, size_t len, Format fmt, int flags);
    int decoded_image_size(int fmt, size_t& len);
    uint32_t image_width() { return _ihdr.width; }
    uint32_t image_height() { return _ihdr.height; }

    int buffer_read(size_t n);

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

  private:
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

    class iHeader {
    public:
      unsigned num_channels() const;
      int calculate_scanline_width(uint32_t swidth, size_t& scanline_width) const;
      int calculate_image_width(int fmt, size_t& len) const;
      int calculate_image_size(int fmt, size_t& len) const;

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

    class Chunk {
    public:
      int is_critical_chunk() const;
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

    struct RowInfo {
      uint32_t scanline_idx;
      uint32_t row_num; /* deinterlaced row index */
      int pass;
      uint8_t filter;
    };

    struct SubImage {
      uint32_t width;
      uint32_t height;
      size_t out_width; /* byte width based on output format */
      size_t scanline_width;
    };

    struct Text2 {
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

    struct DecodeFlagStat {
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

    struct EncodeFlagStat {
      unsigned interlace: 1;
      unsigned same_layout: 1;
      unsigned to_bigendian: 1;
      unsigned progressive: 1;
      unsigned finalize: 1;

      FilterChoice filter_choice;
    };

    struct ChunkStat {
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
    class _iter {
    public:
      _iter(unsigned bit_depth, const unsigned char *samples);
      uint8_t get_sample();

      const uint8_t mask;
      unsigned shift_amount;
      const unsigned initial_shift, bit_depth;
      const unsigned char *samples;
    };

    union DecodePLTE {
      struct PLTEEntry rgba[256];
      unsigned char rgb[256 * 3];
      unsigned char raw[256 * 4];
      uint32_t align_this;
    };

    class ZLIBOptions {
    public:
      ZLIBOptions(int compressionLevel, int windowBits, int memLevel, int strategy, int dataType)
          : _compressionLevel(compressionLevel),
            _windowBits(windowBits),
            _memLevel(memLevel),
            _strategy(strategy),
            _dataType(dataType) {
      }

      int _compressionLevel;
      int _windowBits;
      int _memLevel;
      int _strategy;
      int _dataType;
    };

    void read_chunks();

    int read_data(size_t bytes);

    void read_ihdr();

    int read_and_check_crc();

    int read_header();

    int read_chunk_bytes(uint32_t bytes);

    int discard_chunk_bytes(uint32_t bytes);

    int read_idat_bytes(uint32_t *bytes_read);

    int read_scanline();

    int read_scanline_bytes(unsigned char *dest, size_t len);

    int read_non_idat_chunks();

    int decode_scanline(void *out, size_t len);

    int decode_row(void *out, size_t len);

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

    int encode_chunks();

    int encode_image(const void *img, size_t len, Format fmt, int flags);

    int update_row_info();

    int calculate_subimages();

    int read_chunk_bytes2(void *out, uint32_t bytes);

    int increase_cache_usage(size_t bytes, int new_chunk);

    int decrease_cache_usage(size_t usage);

    int inflate_init(int window_bits);

    int deflate_init(const ZLIBOptions &options);

    int inflate_stream(char **out, size_t *len, size_t extra, const void *start_buf, size_t start_len);

    void splt_undo();

    void text_undo();

    void chunk_undo();

    int set_png_stream(_rw_fn_ptr *rw_func, void *user);

    int set_png_file(FILE *file);

    void *get_png_buffer(size_t *len, int *error);

    int set_image_limits(uint32_t width, uint32_t height);

    int get_image_limits(uint32_t *width, uint32_t *height);

    int set_chunk_limits(size_t chunk_size, size_t cache_size);

    int get_chunk_limits(size_t *chunk_size, size_t *cache_size);

    int set_crc_action(int critical, int ancillary);

    int set_option(enum Option option, int value);

    int get_option(enum Option option, int *value);

    int get_row_info(struct RowInfo *row_info);

    struct iHeader get_ihdr();

    struct PLTE get_plte();

    struct Trns get_trns();

    struct Chrm get_chrm();

    struct ChrmInt get_chrm_int();

    double get_gama();

    uint32_t get_gama_int();

    struct ICCP get_iccp();

    struct SBIT get_sbit();

    uint8_t get_srgb();

    upan::vector<struct TEXT> get_text();

    struct BKGD get_bkgd();

    struct HIST get_hist();

    struct PHYS get_phys();

    struct Time get_time();

    struct Offs get_offs();

    struct Exif get_exif();

    int get_splt(struct SPLT *splt, uint32_t *n_splt);

    int get_unknown_chunks(struct UnknownChunk *chunks, uint32_t *n_chunks);

    int check_ihdr(const struct iHeader &ihdr, uint32_t max_width, uint32_t max_height);
    int check_chrm_int(const struct ChrmInt &chrm_int);
    int check_sbit(const struct SBIT &sbit, const struct iHeader &ihdr);
    int check_time(const struct Time &time);
    int check_offs(const struct Offs &offs);
    int check_exif(const struct Exif &exif);
    int check_png_keyword(const char *str);
    int check_plte(const struct PLTE &plte, const struct iHeader &ihdr);
    int check_phys(const struct PHYS &phys);
    int check_decode_fmt(const int fmt) const;

    int set_ihdr(const struct iHeader &ihdr);
    int set_plte(const struct PLTE &plte);
    int set_trns(const struct Trns &trns);
    int set_chrm(const struct Chrm &chrm);
    int set_chrm_int(const struct ChrmInt &chrm_int);
    int set_gama(double gamma);
    int set_gama_int(uint32_t gamma);
    int set_iccp(const struct ICCP &iccp);
    int set_sbit(const struct SBIT &sbit);
    int set_srgb(uint8_t rendering_intent);
    int set_text(const struct TEXT *text, uint32_t n_text);
    int set_bkgd(const struct BKGD &bkgd);
    int set_hist(const struct HIST &hist);
    int set_phys(const struct PHYS &phys);
    int set_splt(struct SPLT *splt, uint32_t n_splt);
    int set_time(const struct Time &time);
    int set_unknown_chunks(struct UnknownChunk *chunks, uint32_t n_chunks);
    int set_offs(const struct Offs &offs);
    int set_exif(const struct Exif &exif);

    void unpack_scanline(unsigned char *out, const unsigned char *scanline, uint32_t width, unsigned bit_depth, int fmt);

    const char *_strerror(int err);

    int defilter_scanline(const unsigned char *prev_scanline, unsigned char *scanline,
                          size_t scanline_width, unsigned bytes_per_pixel, const unsigned filter);
    int filter_scanline(unsigned char *filtered, const unsigned char *prev_scanline, const unsigned char *scanline,
                        size_t scanline_width, unsigned bytes_per_pixel, const unsigned filter);
    int filter_sum(const unsigned char *prev_scanline, const unsigned char *scanline,
                   size_t size, unsigned bytes_per_pixel, const unsigned filter);
    unsigned get_best_filter(const unsigned char *prev_scanline, const unsigned char *scanline,
                             size_t scanline_width, unsigned bytes_per_pixel, const int choices);
    void gamma_correct_row(unsigned char *row, uint32_t pixels, int fmt, const uint16_t *gamma_lut);
    void trns_row(unsigned char *row, const unsigned char *scanline, const unsigned char *trns,
                  unsigned scanline_stride, iHeader *ihdr, uint32_t pixels, int fmt);
    void scale_row(unsigned char *row, uint32_t pixels, int fmt, unsigned depth, const struct SBIT *sbit);
    void expand_row(unsigned char *row, const unsigned char *scanline, const union DecodePLTE *decode_plte,
        uint32_t width, int fmt);

  private:
    size_t _bytes_read;
    size_t _stream_buf_size;
    byte *_stream_buf;
    const byte *_data;

    /* User-defined pointers for streaming */
    _read_fn_ptr *_read_fn;
    _write_fn_ptr *_write_fn;
    void *_stream_user_ptr;

    /* Used for buffer reads */
    size_t _bytes_left;
    size_t _last_read_size;

    /* Used for encoding */
    int _user_owns_out_png;
    unsigned char *_out_png;
    unsigned char *_write_ptr;
    size_t _out_png_size;
    size_t _bytes_encoded;

    /* These are updated by read/write_header()/read_chunk_bytes() */
    Chunk _current_chunk;
    uint32_t _cur_chunk_bytes_left;
    uint32_t _cur_actual_crc;

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

    struct ZLIBOptions _image_options;
    struct ZLIBOptions _text_options;

    undo_func_ptr _undo;

    /* input file contains this chunk */
    struct ChunkStat _file;

    /* chunk was stored with set_*() */
    struct ChunkStat _user;

    /* chunk was stored by reading or with set_*() */
    struct ChunkStat _stored;

    /* used to reset the above in case of an error */
    struct ChunkStat _prev_stored;

    Chunk _first_idat, _last_idat;

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
    struct Text2 *_text_list;

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

    struct SubImage _subimage[7];

    gccucore::zlib::z_stream _zstream;
    byte *_scanline_buf, *_prev_scanline_buf, *_row_buf, *_filtered_scanline_buf;
    byte *_scanline, *_prev_scanline, *_row, *_filtered_scanline;

    /* based on fmt */
    size_t _image_size; /* may be zero */
    size_t _image_width;

    unsigned _bytes_per_pixel; /* derived from ihdr */
    unsigned _pixel_size; /* derived from _format+ihdr */
    int _widest_pass;
    int _last_pass; /* last non-empty pass */

    uint16_t *_gamma_lut; /* points to either _lut8 or _lut16 */
    uint16_t *_gamma_lut16;
    uint16_t _gamma_lut8[256];
    unsigned char _trns_px[8];
    union DecodePLTE _decode_plte;
    struct SBIT _decode_sb;
    struct DecodeFlagStat _decode_flags;
    struct RowInfo _row_info;

    struct EncodeFlagStat _encode_flags;
  };
}