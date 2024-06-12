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
  RectangleCanvas::RectangleCanvas(int x, int y, int32_t width, int32_t height, HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType)
  : Canvas(x, y, width, height, horizontalPlacementType, verticalPlacementType), _layout(*this) {
  }

  UIObjectImpl::IntersectInfo RectangleCanvas::intersect(int x, int y) const {
    const auto bgAlpha = backgroundColorAlpha();
    const auto brThickness = borderThickness();

    const int x1 = drawX();
    const int x2 = drawX() + width();
    const int y1 = drawY();
    const int y2 = drawY() + height();

    const int ix1 = x1 + brThickness;
    const int ix2 = x2 - brThickness;

    const int iy1 = y1 + brThickness;
    const int iy2 = y2 - brThickness;

    if (brThickness > 0) {
      const auto brAplha = borderColorAlpha();

      if (x < ix1 || x > ix2 || y < iy1 || y > iy2) {
        return { brAplha != 0, x - x1, x2 - x, y - y1, y2 - y };
      } else {
        return { bgAlpha != 0, x - ix1, ix2 - x, y - iy1, iy2 - y };
      }
    } else {
      return { bgAlpha != 0, x - x1, x2 - x, y - y1, y2 - y };
    }
  }

  void RectangleCanvas::doDraw() {
    drawBuffer().cleanBuffer();
    layout().fill();
  }
}