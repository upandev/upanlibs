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

  static void plot(int x, int y, int r, const FrameBuffer& framebuffer, uint32_t color) {
    int sx1 = -x + r;
    int sx2 = x + r;
    int sy = r - y;
    int yoffset = sy * framebuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(framebuffer.buffer()[i + yoffset], color);
    }

    sy = r + y;
    yoffset = sy * framebuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(framebuffer.buffer()[i + yoffset], color);
    }

    sy = x + r;
    sx1 = r - y;
    sx2 = r + y;
    yoffset = sy * framebuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(framebuffer.buffer()[i + yoffset], color);
    }

    sy = -x + r;
    yoffset = sy * framebuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(framebuffer.buffer()[i + yoffset], color);
    }
  }

  void RoundCanvas::fill() {
    const auto alpha = backgroundColorAlpha();
    if (alpha == 0) {
      return;
    }

    const auto& framebuffer = drawBuffer();
    const auto color = (backgroundColorForDraw() & ~GCoreFunctions::ALPHA_MASK) | (alpha << 24);

    const int r = width() / 2;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while(x <= y) {
      plot(x, y, r, framebuffer, color);

      if (d < 0) {
        d += 4 * x + 6;
        ++x;
      } else {
        d += 4 * (x - y) + 10;
        ++x;
        --y;
      }
    }
  }
}