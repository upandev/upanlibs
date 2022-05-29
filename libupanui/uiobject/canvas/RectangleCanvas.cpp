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
  RectangleCanvas::RectangleCanvas(const int x, const int y, const uint32_t width, const uint32_t height)
  : Canvas(x, y, width, height), _layout(*this) {
  }

  bool RectangleCanvas::intersect(int x, int y) const {
    const auto bgAlpha = backgroundColorAlpha();
    const auto brThickness = borderThickness();

    if (brThickness > 0) {
      const auto brAplha = borderColorAlpha();

      const int innerX = drawX() + brThickness;
      const int innerWidth = width() - 2 * brThickness;

      const int innerY = drawY() + brThickness;
      const int innerHeight = height() - 2 * brThickness;

      if (x < innerX || x > (innerX + innerWidth) || y < innerY || y > (innerY + innerHeight)) {
        return brAplha != 0;
      }
    }
    return bgAlpha != 0;
  }

  void RectangleCanvas::doDraw() {
    layout().fill();
  }
}