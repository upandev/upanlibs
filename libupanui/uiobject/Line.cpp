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

#include <Line.h>
#include "GCoreFunctions.h"
#include <math.h>

namespace upanui {
  Line::Line(const int x1, const int y1, const int x2, const int y2)
      : UILeafElement(upan::min(x1, x2), upan::min(y1, y2), abs(x2 - x1), abs(y2 - y1)),
        _x1(x1), _y1(y1), _x2(x2), _y2(y2) {
  }

  void Line::doDraw() {
    const auto alpha = backgroundColorAlpha();
    if (alpha == 0) {
      return;
    }

    const auto bgColor = (backgroundColorForDraw() & ~GCoreFunctions::ALPHA_MASK) | (alpha << 24);
    const int thickness = borderThickness() > 1 ? borderThickness() : 1;
    drawLine(_x1, _y1, _x2, _y2, thickness, bgColor, false);
  }

  void Line::drawLine(const int x1, const int y1, const int x2, const int y2, const uint32_t thickness, const uint32_t color, const bool fillLines) {
    const int dy = abs(y2 - y1);
    const int dx = abs(x2 - x1);
    if (dy < dx) {
      if (x1 > x2) {
        drawLineWithLowSlope(x2, y2, x1, y1, thickness, color, fillLines);
      } else {
        drawLineWithLowSlope(x1, y1, x2, y2, thickness, color, fillLines);
      }
    } else {
      if (y1 > y2) {
        drawLineWithHighSlope(x2, y2, x1, y1, thickness, color, fillLines);
      } else {
        drawLineWithHighSlope(x1, y1, x2, y2, thickness, color, fillLines);
      }
    }
  }

  void Line::plot(const int x, const int y, const uint32_t color) {
    auto& drawBuf = drawBuffer();
    if (x < 0 || y < 0 || x >= parent().width() || y >= parent().height()) {
      return;
    }
    GCoreFunctions::setPixel(drawBuf.at(x, y), color, false);
  }

  void Line::drawLineWithLowSlope(const int sx, const int sy, const int ex, const int ey, const uint32_t thickness, const uint32_t color, const bool fillLines) {
    const int dx = ex - sx;
    int dy = ey - sy;
    int px, py;
    calculatePxPy(px, py, dx, dy, sx, sy, thickness);

    int yinc = 1;
    if (dy < 0) {
      yinc = -1;
      dy = -dy;
    }
    int d = 2 * dy - dx;
    int y = sy;

    bool yChanged = false;
    for(int x = sx; x <= ex; ++x, ++px) {
      if (thickness > 1) {
        drawLine(x, y, px, py, 1, color, true);
      } else {
        plot(x, y, color);
        if (yChanged && fillLines) {
          yChanged = false;
          plot(x, y - yinc, color);
        }
      }
      if (d > 0) {
        y += yinc;
        py += yinc;
        yChanged = true;
        d += 2 * (dy - dx);
      } else {
        d += 2 * dy;
      }
    }
  }

  void Line::drawLineWithHighSlope(const int sx, const int sy, const int ex, const int ey, const uint32_t thickness, const uint32_t color, const bool fillLines) {
    int dx = ex - sx;
    const int dy = ey - sy;
    int px, py;
    calculatePxPy(px, py, dx, dy, sx, sy, thickness);

    int xinc = 1;
    if (dx < 0) {
      xinc = -1;
      dx = -dx;
    }
    int d = 2 * dx - dy;
    int x = sx;

    bool xChanged = false;
    for(int y = sy; y <= ey; ++y, ++py) {
      if (thickness > 1) {
        drawLine(x, y, px, py, 1, color, true);
      } else {
        plot(x, y, color);
        if (xChanged && fillLines) {
          xChanged = false;
          plot(x - xinc, y, color);
        }
      }
      if (d > 0) {
        x += xinc;
        px += xinc;
        xChanged = true;
        d += 2 * (dx - dy);
      } else {
        d += 2 * dx;
      }
    }
  }

  void Line::calculatePxPy(int& px, int& py, const int dx, const int dy, const int sx, const int sy, const uint32_t thickness) {
    px = sx;
    py = sy;
    if (thickness > 1) {
      float m = -float(dx) / float(dy);
      float r = sqrt(1 + m * m);
      px = float(sx) + thickness / r;
      py = float(sy) + thickness * m / r;
    }
  }
}