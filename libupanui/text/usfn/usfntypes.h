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
#pragma once

#include <stdlib.h>

namespace upanui {
  namespace usfn {
    static const int FONT_SIZE_MAX = 192; /* biggest size we can render */
    static const int ITALIC_DIV = 4; /* italic angle divisor, glyph top side pushed width / this pixels */
    static const int DATA_MAX = ((FONT_SIZE_MAX + 4 + (FONT_SIZE_MAX + 4) / ITALIC_DIV) << 8);
    static const int PREC_BITS = 4; /* precision in bits */

    static const char USFN_MAGIC[] = "SFN2";
    static const char USFN_COLLECTION[] = "SFNC";
    static const char USFN_ENDMAGIC[] = "2NFS";

    /* font family group in font type byte */
    static constexpr int TYPE_FAMILY(int x) { return x & 15; }
    static const int FAMILY_SERIF = 0;
    static const int FAMILY_SANS = 1;
    static const int FAMILY_DECOR = 2;
    static const int FAMILY_MONOSPACE = 3;
    static const int FAMILY_HAND = 4;

    static const int FAMILY_ANY = 0xff; /* select the first loaded font */
    static const int FAMILY_BYNAME = 0xfe; /* select font by its unique name */

    /* font style flags in font type byte */
    static constexpr int TYPE_STYLE(int x) { return (x >> 4) & 15; }
    static const int STYLE_REGULAR = 0;
    static const int STYLE_BOLD = 1;
    static const int STYLE_ITALIC = 2;
    static const int STYLE_USRDEF1 = 4; /* user defined variant 1 */
    static const int TYLE_USRDEF2 = 8; /* user defined variant 2 */

    /* additional styles not stored in fonts */
    static const int STYLE_UNDERLINE = 16; /* under line glyph */
    static const int STYLE_STHROUGH = 32; /* strike through glyph */
    static const int STYLE_NOAA = 64; /* no anti-aliasing */
    static const int STYLE_NOKERN = 128; /* no kerning */
    static const int STYLE_NODEFGLYPH = 256; /* don't draw default glyph */
    static const int STYLE_NOCACHE = 512; /* don't cache rasterized glyph */
    static const int STYLE_NOHINTING = 1024; /* no auto hinting grid (not used as of now) */
    static const int STYLE_RTL = 2048; /* render right-to-left */
    static const int STYLE_ABS_SIZE = 4096; /* scale absoulte height */

    /* ligatures area */
    static const int LIG_FIRST = 0xF000;
    static const int LIG_LAST = 0xF8FF;

    /* contour commands */
    static const int CONTOUR_MOVE = 0;
    static const int CONTOUR_LINE = 1;
    static const int CONTOUR_QUAD = 2;
    static const int CONTOUR_CUBIC = 3;

    /* main SSFN header, 32 bytes */
    typedef struct {
      uint8_t magic[4];               /* SSFN magic bytes */
      uint32_t size;                   /* total size in bytes */
      uint8_t type;                   /* font family and style */
      uint8_t features;               /* format features and revision */
      uint8_t width;                  /* overall width of the font */
      uint8_t height;                 /* overall height of the font */
      uint8_t baseline;               /* horizontal baseline in grid pixels */
      uint8_t underline;              /* position of under line in grid pixels */
      uint16_t fragments_offs;         /* offset of fragments table relative to magic */
      uint32_t characters_offs;        /* characters table offset relative to magic */
      uint32_t ligature_offs;          /* ligatures table offset relative to magic */
      uint32_t kerning_offs;           /* kerning table offset relative to magic */
      uint32_t cmap_offs;              /* color map offset relative to magic */
    } PACKED Font;

    /* cached bitmap struct */
    typedef struct {
      uint16_t p;                         /* data buffer pitch, bytes per line */
      uint8_t h;                          /* data buffer height */
      uint8_t o;                          /* overlap of glyph, scaled to size */
      uint8_t x;                          /* advance x, scaled to size */
      uint8_t y;                          /* advance y, scaled to size */
      uint8_t a;                          /* ascender, scaled to size */
      uint8_t d;                          /* descender, scaled to size */
      uint8_t data[DATA_MAX];        /* data buffer */
    } Glyph;

    /* character metrics */
    typedef struct {
      uint8_t t;                          /* type and overlap */
      uint8_t n;                          /* number of fragments */
      uint8_t w;                          /* width */
      uint8_t h;                          /* height */
      uint8_t x;                          /* advance x */
      uint8_t y;                          /* advance y */
    } CharInfo;

    /* destination frame buffer context */
    typedef struct {
      uint8_t *ptr;                       /* pointer to the buffer */
      int16_t w;                          /* width (positive: ARGB, negative: ABGR pixels) */
      int16_t h;                          /* height */
      uint16_t p;                         /* pitch, bytes per line */
      int16_t x;                          /* cursor x */
      int16_t y;                          /* cursor y */
      uint32_t fg;                        /* foreground color */
      uint32_t bg;                        /* background color */
    } FrameBuffer;
  }
}