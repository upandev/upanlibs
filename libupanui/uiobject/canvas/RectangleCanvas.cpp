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

#include <RectangleCanvas.h>
#include <GraphicsContext.h>
#include <GCoreFunctions.h>

namespace upanui {
  RectangleCanvas::RectangleCanvas(const int x, const int y, const uint32_t width, const uint32_t height) : Canvas(x, y, width, height) {
  }

  void RectangleCanvas::fill() {
    const auto alpha = backgroundColorAlpha();
    if (alpha == 0) {
      return;
    }

    const auto& framebuffer = drawBuffer();
    const auto bgColor = (backgroundColorForDraw() & ~GCoreFunctions::ALPHA_MASK) | (alpha << 24);
    const auto brColor = (borderColor() & ~GCoreFunctions::ALPHA_MASK) | (borderColorAlpha() << 24);

    for(auto y = 0u; y < height(); ++y) {
      auto yOffset = y * framebuffer.width();
      if (y < borderThickness() || (height() - y) <= borderThickness()) {
        for(auto x = 0u; x < width(); ++x) {
          GCoreFunctions::setPixel(framebuffer.buffer()[x + yOffset], brColor);
        }
      } else {
        for(auto x = 0u; x < borderThickness(); ++x) {
          GCoreFunctions::setPixel(framebuffer.buffer()[x + yOffset], brColor);
        }
        for(auto x = borderThickness(); x < width() - borderThickness(); ++x) {
          GCoreFunctions::setPixel(framebuffer.buffer()[x + yOffset], bgColor);
        }
        for(auto x = (width() - borderThickness()); x < width(); ++x) {
          GCoreFunctions::setPixel(framebuffer.buffer()[x + yOffset], brColor);
        }
      }
    }
  }
}