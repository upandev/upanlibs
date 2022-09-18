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

#include <usfncontext.h>
#include <exception.h>
#include <uzlib.h>
#include <vector.h>

extern uint32_t _binary_fonts_FreeSans_sfn_start;
extern uint32_t _binary_unifont_sfn_start;
extern uint32_t _binary_u_vga16_sfn_start;

namespace upanui {
  namespace usfn {
    Context::Context() {
      memset(_fnt, 0, sizeof(_fnt));
      memset(_c, 0, sizeof(_c));
      memset(_len, 0, sizeof(_len));

      _s = nullptr;
      _f = nullptr;
      _g = nullptr;
      _p = nullptr;
      _bufs = nullptr;
      _rc = nullptr;
      _numbuf = _lenbuf = _np = _ap = _mx = _my = _lx = _ly = 0;
      _family = _style = _size = _line = 0;
    }

    /**
     * Load a font or font collection into renderer context
     *
     * @param ctx rendering context
     * @param font SSFN font or font collection in memory
     * @return error code
     */
    const uint8_t* Context::GetPreloadedFont(PreloadedFonts type) {
      switch(type) {
        case FreeSans:
          return (uint8_t*)&_binary_fonts_FreeSans_sfn_start;
        case VGA16:
          return (uint8_t*)&_binary_u_vga16_sfn_start;
        case Unifont:
          return (uint8_t*)&_binary_unifont_sfn_start;
      }
      throw upan::exception(XLOC, "unsupported preloaded font: %d", type);
    }

    void Context::Load(const uint8_t* fontData) {
      Font* font = (Font*)fontData;
      Font *fnt, *end;
      int family;
      // GZIP compressed
      if (fontData[0] == 0x1F && fontData[1] == 0x8B) {
        if (fontData[2] != 8) {
          throw upan::exception(XLOC, "invalid gzip compressed font data");
        }
        uint8_t code = fontData[3];
        fontData += 10;
        if(code & 4) {
          uint8_t r = *fontData++;
          r += (*fontData++ << 8);
          fontData += r;
        }
        if(code & 8) {
          while(*fontData++ != 0);
        }
        if(code & 16) {
          while(*fontData++ != 0);
        }
        font = (Font*)zlib::decode((const char*)fontData);
        _bufs = (char**)realloc(_bufs, (_numbuf + 1) * sizeof(char*));
        if (!_bufs) {
          _numbuf = 0;
          throw upan::exception(XLOC, "mem alloc failed for context buffer");
        }
        _bufs[_numbuf++] = (char*)font;
        _lenbuf += font->size;
      }
      if(!memcmp(font->magic, USFN_COLLECTION, 4)) {
        end = (Font*)((uint8_t*)font + font->size);
        for(fnt = (Font*)((uint8_t*)font + 8); fnt < end; fnt = (Font*)((uint8_t*)fnt + fnt->size)) {
          Load((const uint8_t*)fnt);
        }
      } else {
        family = TYPE_FAMILY(font->type);
        //char* xx = (uint8_t*)font + font->size - 4;
        //printf("\n%c%c%c%c", xx[0], xx[1], xx[2], xx[3]);
        if(memcmp(font->magic, USFN_MAGIC, 4) || memcmp((uint8_t*)font + font->size - 4, USFN_ENDMAGIC, 4) ||
        family > FAMILY_HAND || font->fragments_offs >= font->size || font->characters_offs >= font->size ||
        font->ligature_offs >= font->size || font->kerning_offs >= font->size || font->cmap_offs >= font->size ||
        font->fragments_offs >= font->characters_offs) {
          throw upan::exception(XLOC, "bad font file");
        } else {
          _len[family]++;
          _fnt[family] = (const Font**)realloc(_fnt[family], _len[family] * sizeof(void*));
          if(!_fnt[family]) {
            _len[family] = 0;
            throw upan::exception(XLOC, "mem allocation failed while loading font into context");
          } else
            _fnt[family][_len[family] - 1] = font;
        }
        CleanUp();
      }
    }

    /**
   * Set up rendering parameters
   *
   * @param family one of FAMILY_*
   * @param name NULL or UTF-8 string if family is FAMILY_BYNAME
   * @param style OR'd values of STYLE_*
   * @param size how big glyph it should render, 8 - 192
   * @return error code
   */
    void Context::Select(int family, const char *name, int style, int size) {
      CleanUp();

      if((style & ~0x1FFF)) {
        throw upan::exception(XLOC, "usfn: invalid font style: %d", style);
      }

      if(size < 8 || size > FONT_SIZE_MAX) {
        throw upan::exception(XLOC, "usfn: invalid font size: %d", size);
      }

      if(family == FAMILY_BYNAME) {
        if(!name || !name[0]) {
          throw upan::exception(XLOC, "usfn: invalid font name");
        }

        bool familyFound = false;
        int len;
        for(len = 0; name[len]; len++);
        for(int i = 0; i < 5 && !familyFound; i++) {
          for(int j = 0; j < _len[i] && !familyFound; j++) {
            if(!memcmp(name, (uint8_t*)&_fnt[i][j]->magic + sizeof(Font), len)) {
              _s = _fnt[i][j];
              familyFound = true;
            }
          }
        }
        if (!familyFound) {
          throw upan::exception(XLOC, "usfn: font family not found: %d:%s", family, name);
        }
      } else {
        if(family != FAMILY_ANY && (family > FAMILY_HAND || !_len[family])) {
          throw upan::exception(XLOC, "usfn: font family not found: %d", family);
        }
        _s = _fnt[family][0];
      }

      _f = NULL;
      _family = family;
      _style = style;
      _size = size;
      _line = 0;
    }

    // suitable for console
    void Context::RenderCharacter(FrameBuffer& dst, uint32_t unicode) {
#define USFN_PIXEL uint32_t
      register USFN_PIXEL *o, *p;
      register uint8_t *ptr, *chr = NULL, *frg;
      register int i, j, k, l, m, y = 0, w, s = dst.p / sizeof(USFN_PIXEL);

      _f = _s;

      if(!_f || _f->magic[0] != 'S' || _f->magic[1] != 'F' || _f->magic[2] != 'N' || _f->magic[3] != '2' || !dst.ptr || !dst.p) {
        throw upan::exception(XLOC, "invalid font, %d", _f);
      }
      w = dst.w < 0 ? -dst.w : dst.w;
      for(ptr = (uint8_t*)_f + _f->characters_offs, i = 0; i < 0x110000; i++) {
        if(ptr[0] == 0xFF) { i += 65535; ptr++; }
        else if((ptr[0] & 0xC0) == 0xC0) { j = (((ptr[0] & 0x3F) << 8) | ptr[1]); i += j; ptr += 2; }
        else if((ptr[0] & 0xC0) == 0x80) { j = (ptr[0] & 0x3F); i += j; ptr++; }
        else { if((uint32_t)i == unicode) { chr = ptr; break; } ptr += 6 + ptr[1] * (ptr[0] & 0x40 ? 6 : 5); }
      }
      if(!chr) {
        throw upan::exception(XLOC, "no Glyph found");
      }
      ptr = chr + 6; o = (USFN_PIXEL*)((uint8_t*)dst.ptr + dst.y * dst.p + dst.x * sizeof(USFN_PIXEL));
      for(i = 0; i < chr[1]; i++, ptr += chr[0] & 0x40 ? 6 : 5) {
        if(ptr[0] == 255 && ptr[1] == 255) continue;
        frg = (uint8_t*)_f + (chr[0] & 0x40 ? ((ptr[5] << 24) | (ptr[4] << 16) | (ptr[3] << 8) | ptr[2]) :
            ((ptr[4] << 16) | (ptr[3] << 8) | ptr[2]));
        if((frg[0] & 0xE0) != 0x80) continue;
        if(dst.bg) {
          for(; y < ptr[1] && (!dst.h || dst.y + y < dst.h); y++, o += s) {
            for(p = o, j = 0; j < chr[2] && (!w || dst.x + j < w); j++, p++)
              *p = dst.bg;
          }
        } else { o += (int)(ptr[1] - y) * s; y = ptr[1]; }
        k = ((frg[0] & 0x1F) + 1) << 3; j = frg[1] + 1; frg += 2;
        for(m = 1; j && (!dst.h || dst.y + y < dst.h); j--, y++, o += s)
          for(p = o, l = 0; l < k; l++, p++, m <<= 1) {
            if(m > 0x80) { frg++; m = 1; }
            if(dst.x + l >= 0 && (!w || dst.x + l < w)) {
              if(*frg & m) {
                *p = dst.fg;
              } else if(dst.bg) {
                *p = dst.bg;
              }
            }
          }
      }
      if(dst.bg)
        for(; y < chr[3] && (!dst.h || dst.y + y < dst.h); y++, o += s) {
          for(p = o, j = 0; j < chr[2] && (!w || dst.x + j < w); j++, p++)
            *p = dst.bg;
        }
      dst.x += chr[4]; dst.y += chr[5];
    }

    void Context::DrawText(const char *str, FrameBuffer& buf) {
      CalculateTextBufferSize(str, buf);
      const int bufSize = buf.w * buf.h * sizeof(uint32_t);
      if (!bufSize) {
        return;
      }
      buf.p = buf.w * sizeof(uint32_t);
      buf.ptr = (uint8_t*)malloc(bufSize);
      memset(buf.ptr, 0, bufSize);
      //printf("\n%d,%d,%d,%d", buf.w, buf.h, buf.x, buf.y);
      buf.x = 0;
      int ret;
      while((ret = RenderText(buf, str, false)) > 0)
        str += ret;
    }

    void Context::CalculateTextBufferSize(const char *str, FrameBuffer& buf) {
      int ret, f = 1, s;

      if(!str) {
        throw upan::exception(XLOC, "string can't be null");
      }
      buf.w = buf.h = buf.x = buf.y = 0;
      while((ret = RenderText(buf, str, false))) {
        if(ret < 0 || !_g) {
          return;
        }
        if(f) {
          f = 0;
          buf.w = buf.x = _style & STYLE_RTL ? _g->p : _g->o;
        }
        if(_g->x) {
          if(_g->a > buf.y) buf.y = _g->a;
          if(_g->h > buf.h) buf.h = _g->h;
          buf.w += _g->x;
        } else {
          if(buf.w < _g->p) buf.w = _g->p;
          buf.h += _g->y ? _g->y : _g->h;
        }
        str += ret;
      }
      if(_g->x) {
        buf.w += _style & STYLE_RTL ? _g->o : _g->p;
      } else {
        buf.x = buf.w / 2;
        buf.y = 0;
      }
      s = (_style & STYLE_ABS_SIZE) || TYPE_FAMILY(_f->type) == FAMILY_MONOSPACE || !_f->baseline ? _size : _size * _f->height / _f->baseline;
      buf.w = buf.w * s / _f->height;
      buf.h = buf.h * s / _f->height;
      buf.x = _style & STYLE_RTL ? buf.w : (buf.x * s / _f->height);
      //printf("\nbaseline: %d, s: %d, h: %d, size = %d, x = %d", _f->baseline, s, _f->height, _size, buf.x);
      buf.y = buf.y * s / _f->height;
    }

    static int alphaTo100(int a) {
      return (100 * a) / 0xFF;
    }

    static int alphaToFF(int a) {
      return (0xFF * a) / 100;
    }

    int Context::RenderText(FrameBuffer& dst, const char *str, bool fillBG) {
      Font **fl;
      uint8_t *ptr = NULL, *frg, *end, *tmp, color, ci = 0, cb = 0, cs;
      uint16_t r[640];

      uint32_t unicode, P, O, *Op, *Ol;
      unsigned long int sR, sG, sB, sA;
      int ret = 0, i, j, k, l, p, m, n, o, s, x, y, w, h, a, A, b, B, nr, uix, uax;
      int ox, oy, y0, y1, Y0, Y1, x0, x1, X0, X1, X2, xs, ys, yp, pc, af, fB, fG, fR, fA, bB, bG, bR;

      if (!str) {
        throw upan::exception(XLOC, "usfn: invalid display string");
      }

      if (!*str) {
        return 0;
      }

      if(*str == '\r') {
        dst.x = 0;
        return 1;
      }

      if(*str == '\n') {
        dst.x = 0;
        dst.y += _line ? _line : _size;
        return 1;
      }

      /* determine the unicode from str and get a font with a fragment descriptor list for the glyph */
      if(_s) {
        _f = _s;
        ptr = ParseChar(_f, str, &ret, &unicode);
      } else {
        /* find best match */
        p = _family;
        _f = NULL;
        again:  if(p >= FAMILY_BYNAME) { n = 0; m = 4; } else n = m = p;
        for(; n <= m; n++) {
          fl = (Font**)_fnt[n];
          if(_style & 3) {
            /* check if we have a specific _f for the requested style and size */
            for(i = 0; i < _len[n]; i++) {
              if (((fl[i]->type >> 4) & 3) == (_style & 3) && fl[i]->height == _size &&
              (ptr = ParseChar(fl[i], str, &ret, &unicode))) {
                _f = fl[i];
                break;
              }
            }
            /* if not, check if we have the requested size (for bitmap fonts) */
            if(!ptr) {
              for (i = 0; i < _len[n]; i++) {
                if (fl[i]->height == _size && (ptr = ParseChar(fl[i], str, &ret, &unicode))) {
                  _f = fl[i];
                  break;
                }
              }
            }
            /* if neither size+style nor size matched, look for style match */
            if(!ptr) {
              for (i = 0; i < _len[n]; i++) {
                if (((fl[i]->type >> 4) & 3) == (_style & 3) && (ptr = ParseChar(fl[i], str, &ret, &unicode))) {
                  _f = fl[i];
                  break;
                }
              }
            }
            /* if bold italic was requested, check if we have at least bold or italic */
            if(!ptr && (_style & 3) == 3) {
              for (i = 0; i < _len[n]; i++) {
                if (((fl[i]->type >> 4) & 3) && (ptr = ParseChar(fl[i], str, &ret, &unicode))) {
                  _f = fl[i];
                  break;
                }
              }
            }
          }
          /* last resort, get the first _f which has a glyph for this multibyte, no matter style */
          if(!ptr) {
            for(i = 0; i < _len[n]; i++) {
              if ((ptr = ParseChar(fl[i], str, &ret, &unicode))) {
                _f = fl[i];
                break;
              }
            }
          }
        }
        /* if glyph still not found, try any family group */
        if(!ptr && p != FAMILY_ANY) {
          p = FAMILY_ANY;
          goto again;
        }
      }

      if(!ptr) {
        if(_style & STYLE_NODEFGLYPH) {
          throw upan::exception(XLOC, "no Glyph style");
        }
        else {
          /* get the first font in family which has a default glyph */
          unicode = 0;
          if(_family >= FAMILY_BYNAME) {
            n = 0; m = 4;
          } else {
            n = m = _family;
          }
          for(; n <= m && !ptr; n++) {
            if (_len[n] && _fnt[n][0] &&
            !(*((uint8_t *) _fnt[n][0] + _fnt[n][0]->characters_offs) & 0x80)) {
              _f = _fnt[n][0];
              ptr = (uint8_t *) _f + _f->characters_offs;
            }
          }
        }
        if(!ptr) {
          throw upan::exception(XLOC, "couldn't find Glyph");
        }
      }

      if(!_f || !_f->height || !_size) {
        throw upan::exception(XLOC, "no font loaded in context");
      }

      if((unicode >> 16) > 0x10) {
        throw upan::exception(XLOC, "failed to render %s", str);
      }

      _rc = (CharInfo*)ptr;
      ptr += sizeof(CharInfo);

      /* render glyph into cache */
      if(!(_style & STYLE_NOCACHE) && _c[unicode >> 16] && _c[unicode >> 16][(unicode >> 8) & 0xFF] &&
      _c[unicode >> 16][(unicode >> 8) & 0xFF][unicode & 0xFF]) {
        _g = _c[unicode >> 16][(unicode >> 8) & 0xFF][unicode & 0xFF];
      } else {
        h = _style & STYLE_NOAA ? _size : (_size > _f->height ? (_size + 4) & ~3 : _f->height);
        ci = (_style & STYLE_ITALIC) && !(TYPE_STYLE(_f->type) & STYLE_ITALIC);
        cb = (_style & STYLE_BOLD) && !(TYPE_STYLE(_f->type) & STYLE_BOLD) ? (_f->height+64)>>6 : 0;
        w = _rc->w * h / _f->height;
        p = w + (ci ? h / ITALIC_DIV : 0) + cb;
        /* failsafe, should never happen */
        if(p * h >= DATA_MAX) {
          throw upan::exception(XLOC, "usfn: render failed - invalid size");
        }

        if(!(_style & STYLE_NOCACHE)) {
          if(!_c[unicode >> 16]) {
            _c[unicode >> 16] = (Glyph***)malloc(256 * sizeof(void*));
            if(!_c[unicode >> 16]) {
              throw upan::exception(XLOC, "usfn: failed to render - mem allocation failed");
            }
            memset(_c[unicode >> 16], 0, 256 * sizeof(void*));
          }
          if(!_c[unicode >> 16][(unicode >> 8) & 0xFF]) {
            _c[unicode >> 16][(unicode >> 8) & 0xFF] = (Glyph**)malloc(256 * sizeof(void*));
            if(!_c[unicode >> 16][(unicode >> 8) & 0xFF]) {
              throw upan::exception(XLOC, "usfn: failed to render - mem allocation failed");
            }
            memset(_c[unicode >> 16][(unicode >> 8) & 0xFF], 0, 256 * sizeof(void*));
          }
          _g = _c[unicode >> 16][(unicode >> 8) & 0xFF][unicode & 0xFF] = (Glyph*)malloc(p * h + 8);
          if(!_c[unicode >> 16][(unicode >> 8) & 0xFF][unicode & 0xFF]) {
            throw upan::exception(XLOC, "usfn: failed to render - mem allocation failed");
          }
        } else {
          _g = &_ga;
        }

        x = (_rc->x > 0 && ci ? (_f->height - _f->baseline) * h / ITALIC_DIV / _f->height : 0);
        _g->p = p;
        _g->h = h;
        _g->x = _rc->x + x;
        _g->y = _rc->y;
        _g->o = (_rc->t & 0x3F) + x;
        memset(&_g->data, 0xFF, p * h);
        color = 0xFE; _g->a = _g->d = 0;
        for(n = 0; n < _rc->n; n++) {
          if(ptr[0] == 255 && ptr[1] == 255) { color = ptr[2]; ptr += _rc->t & 0x40 ? 6 : 5; continue; }
          x = ((ptr[0] + cb) << PREC_BITS) * h / _f->height; y = (ptr[1] << PREC_BITS) * h / _f->height;
          if(_rc->t & 0x40) { m = (ptr[5] << 24) | (ptr[4] << 16) | (ptr[3] << 8) | ptr[2]; ptr += 6; }
          else { m = (ptr[4] << 16) | (ptr[3] << 8) | ptr[2]; ptr += 5; }
          frg = (uint8_t*)_f +  m;
          if(!(frg[0] & 0x80)) {
            /* contour */
            j = (frg[0] & 0x3F);
            if(frg[0] & 0x40) { j <<= 8; j |= frg[1]; frg++; }
            j++; frg++; tmp = frg; frg += (j+3)/4; _np = 0;
            for(i = 0; i < j; i++) {
              k = (frg[0] << PREC_BITS) * h / _f->height + x; m = (frg[1] << PREC_BITS) * h / _f->height + y;
              switch((tmp[i >> 2] >> ((i & 3) << 1)) & 3) {
                case CONTOUR_MOVE:
                  _mx = _lx = k; _my = _ly = m;
                  frg += 2;
                  break;
                  case CONTOUR_LINE:
                    AddLineToContour(p << PREC_BITS, h << PREC_BITS, k, m);
                    frg += 2;
                    break;
                    case CONTOUR_QUAD:
                      a = (frg[2] << PREC_BITS) * h / _f->height + x; A = (frg[3] << PREC_BITS) * h / _f->height + y;
                      AddBezierCurveToContour(p << PREC_BITS,h << PREC_BITS, _lx, _ly,
                                              ((a-_lx)/2)+_lx, ((A-_ly)/2)+_ly,
                                              ((k-a)/2)+a, ((A-m)/2)+m,
                                              k, m, 0);
                      frg += 4;
                      break;
                      case CONTOUR_CUBIC:
                        a = (frg[2] << PREC_BITS) * h / _f->height + x; A = (frg[3] << PREC_BITS) * h / _f->height + y;
                        b = (frg[4] << PREC_BITS) * h / _f->height + x; B = (frg[5] << PREC_BITS) * h / _f->height + y;
                        AddBezierCurveToContour(p << PREC_BITS,h << PREC_BITS, _lx, _ly, a, A, b, B, k, m, 0);
                        frg += 6;
                        break;
              }
            }
            /* close path */
            if(_mx != _lx || _my != _ly) {
              _p[_np+0] = _mx;
              _p[_np+1] = _my;
              _np += 2;
            }
            /* add rasterized vector layers to cached glyph */
            if(_np > 4) {
              for(b = A = B = o = 0; b < h; b++, B += p) {
                a = b << PREC_BITS;
                for(nr = 0, i = 0; i < _np - 3; i += 2) {
                  if( (_p[i+1] < a && _p[i+3] >= a) ||
                  (_p[i+3] < a && _p[i+1] >= a)) {
                    if((_p[i+1] >> PREC_BITS) == (_p[i+3] >> PREC_BITS))
                      x = (((int)_p[i]+(int)_p[i+2])>>1);
                    else
                      x = ((int)_p[i]) + ((a - (int)_p[i+1])*
                          ((int)_p[i+2] - (int)_p[i])/
                          ((int)_p[i+3] - (int)_p[i+1]));
                    x >>= PREC_BITS;
                    if(ci) x += (h - b) / ITALIC_DIV;
                    if(cb && !o) {
                      if(_g->data[B + x] != color) { o = -cb; A = cb; }
                      else { o = cb; A = -cb; }
                    }
                    for(k = 0; k < nr && x > r[k]; k++);
                    for(l = nr; l > k; l--) r[l] = r[l-1];
                    r[k] = x;
                    nr++;
                  }
                }
                if(nr > 1 && nr & 1) { r[nr - 2] = r[nr - 1]; nr--; }
                if(nr) {
                  if(_g->d < y + b) _g->d = y + b;
                  for(i = 0; i < nr - 1; i += 2) {
                    l = r[i] + o; m = r[i + 1] + A;
                    if(l < 0) l = 0;
                    if(m > p) m = p;
                    if(i > 0 && l < r[i - 1] + A) l = r[i - 1] + A;
                    for(; l < m; l++)
                      _g->data[B + l] = _g->data[B + l] == color ? 0xFF : color;
                  }
                }
              }
            }
          } else if((frg[0] & 0x60) == 0x00) {
            /* bitmap */
            B = ((frg[0] & 0x1F) + 1) << 3; A = frg[1] + 1; x >>= PREC_BITS; y >>= PREC_BITS;
            b = B * h / _f->height; a = A * h / _f->height;
            if(_g->d < y + a) _g->d = y + a;
            frg += 2;
            for(j = 0; j < a; j++) {
              k = j * A / a;
              l = (y + j) * p + x + (ci ? (h - y - j) / ITALIC_DIV : 0);
              for(i = 0; i < b; i++) {
                m = i * B / b;
                if(frg[(k * B + m) >> 3] & (1 << (m & 7))) {
                  for(o = 0; o <= cb; o++)
                    _g->data[l + i + o] = color;
                }
              }
            }
            if(!(_style & STYLE_NOAA)) {
              m = color == 0xFD ? 0xFC : 0xFD; o = y * p + p + x;
              for(k = h; k > _f->height + 4; k -= 2*_f->height) {
                for(j = 1, l = o; j < a - 1; j++, l += p)
                  for(i = 1; i < b - 1; i++) {
                    if(_g->data[l + i] == 0xFF && (_g->data[l + i - p] == color ||
                    _g->data[l + i + p] == color) && (_g->data[l + i - 1] == color ||
                    _g->data[l + i + 1] == color)) _g->data[l + i] = m;
                  }
                for(j = 1, l = o; j < a - 1; j++, l += p)
                  for(i = 1; i < b - 1; i++) {
                    if(_g->data[l + i] == m) _g->data[l + i] = color;
                  }
              }
            }
          } else if((frg[0] & 0x60) == 0x20) {
            /* pixmap */
            k = (((frg[0] & 0x1F) << 8) | frg[1]) + 1; B = frg[2] + 1; A = frg[3] + 1; x >>= PREC_BITS; y >>= PREC_BITS;
            b = B * h / _f->height; a = A * h / _f->height;
            if(_g->d < y + a) _g->d = y + a;
            frg += 4; end = frg + k;
            upan::vector<uint32_t> dec;
            while(frg < end) {
              l = ((*frg++) & 0x7F) + 1;
              if(frg[-1] & 0x80) {
                while(l--) dec.push_back(*frg);
                frg++;
              } else while(l--) dec.push_back(*frg++);
            }
            for(j = 0; j < a; j++) {
              k = j * A / a * B;
              l = (y + j) * p + x + (ci ? (h - y - j) / ITALIC_DIV : 0);
              for(i = 0; i < b; i++) {
                m = dec[k + i * B / b];
                if(m != 0xFF) _g->data[l + i] = m;
              }
            }
          }
          color = 0xFE;
        }
        _g->a = _f->baseline;
        if(_g->d > _g->a + 1) _g->d -= _g->a + 1; else _g->d = 0;
      }
    
      /* blit glyph from cache into buffer */
      h = (_style & STYLE_ABS_SIZE) || TYPE_FAMILY(_f->type) == FAMILY_MONOSPACE || !_f->baseline ?
          _size : _size * _f->height / _f->baseline;
      if(h > _line) _line = h;
      w = _g->p * h / _g->h;
      s = _g->x * h / _f->height - _g->o * h / _f->height;
      n = _size > 16 ? 2 : 1;
      if(w < n) w = n;
      if(s < n) s = n;
      if(dst.ptr) {
        if(_g->x) {
          ox = (_g->o * h / _f->height) + (_style & STYLE_RTL ? w : 0);
          oy = _g->a * h / _f->height;
        } else { ox = w / 2; oy = 0; }
        j = dst.w < 0 ? -dst.w : dst.w;
        cs = dst.w < 0 ? 16 : 0;
        cb = (h + 64) >> 6; uix = w > s ? w : s; uax = 0;
        n = _f->underline * h / _f->height;
        fR = (dst.fg >> 16) & 0xFF; fG = (dst.fg >> 8) & 0xFF; fB = (dst.fg >> 0) & 0xFF; fA = (dst.fg >> 24) & 0xFF;
        //upanix uses alpha as a percent from 0 to 100 - so convert alpha to 0 to 0xFF range
        fA = alphaToFF(fA);
        bR = (dst.bg >> 16) & 0xFF; bG = (dst.bg >> 8) & 0xFF; bB = (dst.bg >> 0) & 0xFF;
        Op = (uint32_t*)(dst.ptr + dst.p * (dst.y - oy) + ((dst.x - ox) << 2));

        for (y = 0; y < h && dst.y + y - oy < dst.h; y++, Op += dst.p >> 2) {
          if(dst.y + y - oy < 0) continue;
          y0 = (y << 8) * _g->h / h; Y0 = y0 >> 8; y1 = ((y + 1) << 8) * _g->h / h; Y1 = y1 >> 8; Ol = Op;
          for (x = 0; x < w && dst.x + x - ox < j; x++, Ol++) {
            if(dst.x + x - ox < 0) continue;
            m = 0; sR = sG = sB = sA = 0;
            if(!dst.bg) {
              /* real linear frame buffers should be accessed only as uint32_t on 32 bit boundary */
              O = *Ol;
              bR = (O >> (16 - cs)) & 0xFF;
              bG = (O >> 8) & 0xFF;
              bB = (O >> cs) & 0xFF;
            }
            x0 = (x << 8) * _g->p / w; X0 = x0 >> 8; x1 = ((x + 1) << 8) * _g->p / w; X1 = x1 >> 8;
            for(ys = y0; ys < y1; ys += 256) {
              if(ys >> 8 == Y0) { yp = 256 - (ys & 0xFF); ys &= ~0xFF; if(yp > y1 - y0) yp = y1 - y0; }
              else if(ys >> 8 == Y1) yp = y1 & 0xFF;
              else yp = 256;
              X2 = (ys >> 8) * _g->p;
              for(xs = x0; xs < x1; xs += 256) {
                if (xs >> 8 == X0) {
                  k = 256 - (xs & 0xFF); xs &= ~0xFF; if(k > x1 - x0) k = x1 - x0;
                  pc = k == 256 ? yp : (k * yp) >> 8;
                } else
                  if (xs >> 8 == X1) {
                    k = x1 & 0xFF; pc = k == 256 ? yp : (k * yp) >> 8;
                  } else {
                    pc = yp;
                  }
                  m += pc;
                  k = _g->data[X2 + (xs >> 8)];
                  if(k == 0xFF) {
                    sB += bB * pc; sG += bG * pc; sR += bR * pc;
                  } else
                    if(k == 0xFE || !_f->cmap_offs) {
                      af = (256 - fA) * pc;
                      sB += fB * af; sG += fG * af; sR += fR * af; sA += fA * pc;
                    } else {
                      P = *((uint32_t*)((uint8_t*)_f + _f->cmap_offs + (k << 2)));
                      af = (256 - (P >> 24)) * pc;
                      sR += (((P >> 16) & 0xFF) * af);
                      sG += (((P >> 8) & 0xFF) * af);
                      sB += (((P >> 0) & 0xFF) * af);
                      sA += (((P >> 24) & 0xFF) * pc);
                    }
              }
            }
            if(m) { sR /= m; sG /= m; sB /= m; sA /= m; }
            else { sR >>= 8; sG >>= 8; sB >>= 8; sA >>= 8; }
            if(sA > 15 || fillBG) {
              if (sA > 255) sA = 255;
              //upanix uses alpha as a percent from 0 to 100 - so convert alpha to 0 to 0xFF range
              *Ol = (alphaTo100(sA) << 24) |
                  ((sR > 255 ? 255 : sR) << (16 - cs)) |
                  ((sG > 255 ? 255 : sG) << 8) |
                  ((sB > 255 ? 255 : sB) << cs);
              if(y == n) {
                if(uix > x) uix = x;
                if(uax < x) uax = x;
              }
            }
          }
        }
        if(_style & STYLE_UNDERLINE) {
          uix -= cb + 1; uax += cb + 2;
          if(uax < uix) uax = uix + 1;
          k = (w > s ? w : s);
          Op = (uint32_t*)(dst.ptr + dst.p * (dst.y - oy + n) + ((dst.x - ox - 1) << 2));
          for (y = n; y < n + cb && dst.y + y - oy < dst.h; y++, Op += dst.p >> 2) {
            if(dst.y + y - oy < 0) continue;
            for (Ol = Op, x = 0; x <= k && dst.x + x - ox < j; x++, Ol++) {
              if(dst.x + x - ox < 0 || (x > uix && x < uax)) continue;
              O = *Ol;
              bR = (O >> (16 - cs)) & 0xFF;
              bG = (O >> 8) & 0xFF;
              bB = (O >> cs) & 0xFF;
              bB += ((fB - bB) * fA) >> 8;
              bG += ((fG - bG) * fA) >> 8;
              bR += ((fR - bR) * fA) >> 8;
              *Ol = (alphaTo100(fA) << 24) | (bR << (16 - cs)) | (bG << 8) | (bB << cs);
            }
          }
        }
        if(_style & STYLE_STHROUGH) {
          n = (h >> 1); k = (w > s ? w : s) + 1;
          Op = (uint32_t*)(dst.ptr + dst.p * (dst.y - oy + n) + ((dst.x - ox - 1) << 2));
          for (y = n; y < n + cb && dst.y + y - oy < dst.h; y++, Op += dst.p >> 2) {
            if(dst.y + y - oy < 0) continue;
            for (Ol = Op, x = 0; x <= k && dst.x + x - ox < j; x++, Ol++) {
              if(dst.x + x - ox < 0) continue;
              O = *Ol;
              bR = (O >> (16 - cs)) & 0xFF;
              bG = (O >> 8) & 0xFF;
              bB = (O >> cs) & 0xFF;
              bB += ((fB - bB) * fA) >> 8;
              bG += ((fG - bG) * fA) >> 8;
              bR += ((fR - bR) * fA) >> 8;
              *Ol = (alphaTo100(fA) << 24) | (bR << (16 - cs)) | (bG << 8) | (bB << cs);
            }
          }
        }
      }
      /* add advance and kerning */
      dst.x += (_style & STYLE_RTL ? -s : s);
      dst.y += _g->y * h / _f->height;
      ptr = (uint8_t*)str + ret;
      if(!(_style & STYLE_NOKERN) && _f->kerning_offs && ParseChar(_f, (const char*)ptr, &i, &P) && P > 32) {
        ptr = (uint8_t*)_rc + sizeof(CharInfo);
        /* check all kerning fragments, because we might have both vertical and horizontal kerning offsets */
        for(n = 0; n < _rc->n; n++) {
          if(ptr[0] == 255 && ptr[1] == 255) { ptr += _rc->t & 0x40 ? 6 : 5; continue; }
          x = ptr[0];
          if(_rc->t & 0x40) { m = (ptr[5] << 24) | (ptr[4] << 16) | (ptr[3] << 8) | ptr[2]; ptr += 6; }
          else { m = (ptr[4] << 16) | (ptr[3] << 8) | ptr[2]; ptr += 5; }
          frg = (uint8_t*)_f +  m;
          if((frg[0] & 0xE0) == 0xC0) {
            k = (((frg[0] & 0x1F) << 8) | frg[1]) + 1; frg += 2;
            /* check if there's a kerning group for the next code point */
            while(k--) {
              m = ((frg[2] & 0xF) << 16) | (frg[1] << 8) | frg[0];
              if(P >= (uint32_t)m && P <= (uint32_t)(((frg[5] & 0xF) << 16) | (frg[4] << 8) | frg[3])) {
                P -= m;
                m = _f->kerning_offs + ((((frg[2] >> 4) & 0xF) << 24) | (((frg[5] >> 4) & 0xF) << 16) |
                    (frg[7] << 8) | frg[6]);
                /* decode RLE compressed offsets */
                tmp = (uint8_t*)_f + m;
                while(tmp < (uint8_t*)_f + _f->size - 4) {
                  if((tmp[0] & 0x7F) < P) {
                    P -= (tmp[0] & 0x7F) + 1;
                    tmp += 2 + (tmp[0] & 0x80 ? 0 : tmp[0] & 0x7F);
                  } else {
                    y = (int)((signed char)tmp[1 + ((tmp[0] & 0x80) ? 0 : P)]) * h / _f->height;
                    if(x) dst.x += y; else dst.y += y;
                    break;
                  }
                }
                break;
              }
              frg += 8;
            }
          } /* if kerning fragment */
        }
      }
      return ret;
    }

    /* add a line to contour */
    void Context::AddLineToContour(int p, int h, int x, int y) {
      if(x < 0 || y < 0 || x >= p || y >= h || (
          ((_lx + (1 << (PREC_BITS-1))) >> PREC_BITS) == ((x + (1 << (PREC_BITS-1))) >> PREC_BITS) &&
          ((_ly + (1 << (PREC_BITS-1))) >> PREC_BITS) == ((y + (1 << (PREC_BITS-1))) >> PREC_BITS))) return;
      if(_ap <= _np) {
        _ap = _np + 512;
        _p = (uint16_t*)realloc(_p, _ap * sizeof(uint16_t));
        if(!_p) { _ap = _np = 0; return; }
      }

      if(!_np) {
        _p[0] = _mx;
        _p[1] = _my;
        _np += 2;
      }
      _p[_np+0] = x;
      _p[_np+1] = y;
      _np += 2;
      _lx = x; _ly = y;
    }

    /* add a Bezier curve to contour */
    void Context::AddBezierCurveToContour(int p, int h, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int l) {
      int m0x, m0y, m1x, m1y, m2x, m2y, m3x, m3y, m4x, m4y,m5x, m5y;
      if(l<4 && (x0!=x3 || y0!=y3)) {
        m0x = ((x1-x0)/2) + x0;     m0y = ((y1-y0)/2) + y0;
        m1x = ((x2-x1)/2) + x1;     m1y = ((y2-y1)/2) + y1;
        m2x = ((x3-x2)/2) + x2;     m2y = ((y3-y2)/2) + y2;
        m3x = ((m1x-m0x)/2) + m0x;  m3y = ((m1y-m0y)/2) + m0y;
        m4x = ((m2x-m1x)/2) + m1x;  m4y = ((m2y-m1y)/2) + m1y;
        m5x = ((m4x-m3x)/2) + m3x;  m5y = ((m4y-m3y)/2) + m3y;
        AddBezierCurveToContour(p, h, x0, y0, m0x, m0y, m3x, m3y, m5x, m5y, l+1);
        AddBezierCurveToContour(p, h, m5x, m5y, m4x, m4y, m2x, m2y, x3, y3, l+1);
      }
      if(l) AddLineToContour(p, h, x3, y3);
    }

    /* parse character table */
    uint8_t* Context::ParseChar(const Font *font, const char *str, int *len, uint32_t *unicode) {
      uint32_t i, j, u = -1U;
      uint16_t *l;
      uint8_t *ptr, *s;

      *len = 0; *unicode = 0;
      if(!font || !font->characters_offs || !str || !*str) return NULL;

      if(font->ligature_offs) {
        for(l = (uint16_t*)((uint8_t*)font + font->ligature_offs), i = 0; l[i] && u == -1U; i++) {
          for(ptr = (uint8_t*)font + l[i], s = (uint8_t*)str; *ptr && *ptr == *s; ptr++, s++);
          if(!*ptr) { u = LIG_FIRST + i; *len = (int)(s - (uint8_t*)str); break; }
        }
      }
      if(u == -1U) {
        s = (uint8_t*)str;
        u = DecodeUTF8((char**)&s);
        *len = (int)(s - (uint8_t*)str);
      }
      *unicode = u;
      for(ptr = (uint8_t*)font + font->characters_offs, i = 0; i < 0x110000; i++) {
        if(ptr[0] == 0xFF) { i += 65535; ptr++; }
        else if((ptr[0] & 0xC0) == 0xC0) { j = (((ptr[0] & 0x3F) << 8) | ptr[1]); i += j; ptr += 2; }
        else if((ptr[0] & 0xC0) == 0x80) { j = (ptr[0] & 0x3F); i += j; ptr++; }
        else {
          if(i == u) return ptr;
          ptr += 6 + ptr[1] * (ptr[0] & 0x40 ? 6 : 5);
        }
      }
      return NULL;
    }

    /**
   * Decode an UTF-8 multibyte, advance string pointer and return UNICODE. Watch out, no input checks
   *
   * @param **s pointer to an UTF-8 string pointer
   * @return unicode, and *s moved to next multibyte sequence
   */
    uint32_t Context::DecodeUTF8(char **s) {
      uint32_t c = **s;

      if((**s & 128) != 0) {
        if(!(**s & 32)) { c = ((**s & 0x1F)<<6)|(*(*s+1) & 0x3F); *s += 1; } else
          if(!(**s & 16)) { c = ((**s & 0xF)<<12)|((*(*s+1) & 0x3F)<<6)|(*(*s+2) & 0x3F); *s += 2; } else
            if(!(**s & 8)) { c = ((**s & 0x7)<<18)|((*(*s+1) & 0x3F)<<12)|((*(*s+2) & 0x3F)<<6)|(*(*s+3) & 0x3F); *s += 3; }
            else c = 0;
      }
      *s += 1; /* *s++ is not what you think */
      return c;
    }

    void Context::CleanUp() {
      for(int k = 0; k <= 16; k++) {
        if (_c[k]) {
          for (int j = 0; j < 256; j++) {
            if (_c[k][j]) {
              for (int i = 0; i < 256; i++) {
                if (_c[k][j][i]) {
                  free(_c[k][j][i]);
                }
              }
              free(_c[k][j]);
            }
          }
          free(_c[k]);
          _c[k] = NULL;
        }
      }
    }
  }
}