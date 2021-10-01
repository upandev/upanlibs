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

#include <usfntypes.h>

namespace upanui {
  namespace usfn {
    class Context {
    public:
      Context();
      static const uint8_t* GetPreloadedFont(PreloadedFonts type);

      void Load(const uint8_t* fontData);
      void Select(int family, const char *name, int style, int size);
      void RenderCharacter(FrameBuffer& dst, uint32_t unicode);
      int Render(FrameBuffer& dst, const char *str, bool fillBG);

    private:
      void AddLineToContour(int p, int h, int x, int y);
      void AddBezierCurveToContour(int p, int h, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, int l);
      uint8_t* ParseChar(const Font *font, const char *str, int *len, uint32_t *unicode);
      uint32_t DecodeUTF8(char **s);
      void CleanUp();

      const Font **_fnt[5];         /* dynamic font registry */
      const Font *_s;               /* explicitly selected font */
      const Font *_f;               /* font selected by best match */
      Glyph _ga;                    /* glyph sketch area */
      Glyph *_g;                    /* current glyph pointer */

      Glyph ***_c[17];              /* glyph cache */
      uint16_t *_p;
      char **_bufs;                        /* allocated extra buffers */

      CharInfo *_rc;                     /* pointer to current character */
      int _numbuf, _lenbuf, _np, _ap;
      int _mx, _my, _lx, _ly;                 /* move to coordinates, last coordinates */
      int _len[5];                         /* number of fonts in registry */
      int _family;                         /* required family */
      int _style;                          /* required style */
      int _size;                           /* required size */
      int _line;                           /* calculate line height */
    };
  }
}