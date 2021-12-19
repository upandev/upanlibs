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

#include <RoundCanvas.h>
#include <GraphicsContext.h>
#include "GCoreFunctions.h"

namespace upanui {
  RoundCanvas::RoundCanvas(const int x, const int y, const uint32_t width, const uint32_t height) : Canvas(x, y, width, height) {
  }

  void RoundCanvas::fill() {
    const auto alpha = backgroundColorAlpha();
    if (alpha == 0) {
      return;
    }

    const auto& framebuffer = drawBuffer();
    const auto color = (backgroundColorForDraw() & ~GCoreFunctions::ALPHA_MASK) | (alpha << 24);

    const int r = width() / 2;
    const int r2 = r * r;

    int sx = r;

    for(int sy = 0; sy < height() && sx >= 0;) {
      int ex = sx + (r - sx) * 2;
      int y1Offset = sy * framebuffer.width();
      int y2Offset = (height() - sy - 1) * framebuffer.width();

      for(int x = sx; x <= ex; ++x) {
        framebuffer.buffer()[x + y1Offset] = color;
        if (y1Offset < y2Offset) {
          framebuffer.buffer()[x + y2Offset] = color;
        }
      }

      --sx;
      if (sx < 0) {
        break;
      }
      int sx2 = (sx - r) * (sx - r);
      int sy2 = (sy - r) * (sy - r);
      if ((sx2 + sy2) > r2) {
        ++sy;
        sy2 = (sy - r) * (sy - r);
        if ((sx2 + sy2) > r2) {
          ++sx;
        }
      }
    }
  }
}