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

    const auto rawColor = backgroundColorForDraw() & ~GCoreFunctions::ALPHA_MASK;
    const int thickness = borderThickness() > 1 ? borderThickness() : 1;
    drawLine(_x1, _y1, _x2, _y2, thickness, rawColor, backgroundColorAlpha(), false);
  }

  void Line::drawLine(const int x1, const int y1, const int x2, const int y2, const uint32_t thickness,
                      const uint32_t rawColor, const uint32_t alpha, const bool fillLines) {
    const int dy = abs(y2 - y1);
    const int dx = abs(x2 - x1);
    if (dy < dx) {
      if (x1 > x2) {
        drawLineWithLowSlope(x2, y2, x1, y1, thickness, rawColor, alpha, fillLines);
      } else {
        drawLineWithLowSlope(x1, y1, x2, y2, thickness, rawColor, alpha, fillLines);
      }
    } else {
      if (y1 > y2) {
        drawLineWithHighSlope(x2, y2, x1, y1, thickness, rawColor, alpha, fillLines);
      } else {
        drawLineWithHighSlope(x1, y1, x2, y2, thickness, rawColor, alpha, fillLines);
      }
    }
  }

  void Line::plot(const int x, const int y, const uint32_t color, const uint32_t alpha) {
    auto& drawBuf = drawBuffer();
    if (x < 0 || y < 0 || x >= parent().width() || y >= parent().height()) {
      return;
    }
    GCoreFunctions::setPixel(drawBuf.at(x, y), color | (alpha << 24), false);
  }

  void Line::drawLineWithLowSlope(const int sx, const int sy, const int ex, const int ey, const uint32_t thickness,
                                  const uint32_t rawColor, const uint32_t alpha, const bool fillLines) {
    const int dx = ex - sx;
    const int dy = ey - sy;
    if (dy == 0) {
      drawHorizontalLine(sx, ex, sy, thickness, rawColor, alpha);
      return;
    }
    const float m = float(dy) / float(dx);

    int pex, pey;
    calculatePxPy(pex, pey, dx, dy, sx, sy, thickness);
    const int pdx = pex - sx;
    const int pdy = pey - sy;
    //inverse of slope to calculate actual X on perpendicular line
    const float pm = float(pdx) / float(pdy);

    const int width = ex - sx;

    float apx = sx;

    for(int py = sy; py <= pey; ++py) {
      const int px1 = apx;
      const int px2 = px1 + width;

      const float pe = apx - px1;
      const float pre = 1 - pe;
      const uint32_t palpha1 = alpha * pre;
      const uint32_t palpha2 = alpha * pe;

      plot(px1, py, rawColor, palpha1);
      plot(px1 + 1, py, rawColor, palpha2);

      float ay = py;
      for(int x = px1 + 1; x <= px2; ++x) {
        ay += m;
        int y = ay;
        //border line
        if (py == sy || py == pey) {
          const float e = ay - y;
          const float re = 1 - e;
          const uint32_t alpha1 = alpha * re;
          const uint32_t alpha2 = alpha * e;

          plot(x, y, rawColor, py == sy ? alpha1 : alpha);
          plot(x, y + 1, rawColor, py == pey ? alpha2 : alpha);
        } else {
          plot(x, y, rawColor, alpha);
          plot(x, y + 1, rawColor, alpha);
        }
      }

      plot(px2 + 1, ay, rawColor, palpha2);

      apx -= pm;
    }
  }

  void Line::drawLineWithHighSlope(const int sx, const int sy, const int ex, const int ey, const uint32_t thickness,
                                   const uint32_t rawColor, const uint32_t alpha, const bool fillLines) {
    const int dx = ex - sx;
    const int dy = ey - sy;
    if (dx == 0) {
      drawVerticalLine(sy, ey, sx, thickness, rawColor, alpha);
      return;
    }
    //inverse of slope to calculate actual X
    const float m = float(dx) / float(dy);

    int pex, pey;
    calculatePxPy(pex, pey, dx, dy, sx, sy, thickness);
    const int pdx = pex - sx;
    const int pdy = pey - sy;
    const float pm = float(pdy) / float(pdx);

    const int height = ey - sy;

    float apy = sy;

    for(int px = sx; px <= pex; ++px) {
      const int py1 = apy;
      const int py2 = py1 + height;

      const float pe = apy - py1;
      const float pre = 1 - pe;
      const uint32_t palpha1 = alpha * pre;
      const uint32_t palpha2 = alpha * pe;

      plot(px, py1, rawColor, palpha1);
      plot(px, py1 + 1, rawColor, palpha2);

      float ax = px;
      for(int y = py1 + 1; y <= py2; ++y) {
        ax += m;
        int x = ax;
        //border line
        if (px == sx || px == pex) {
          const float e = ax - x;
          const float re = 1 - e;
          const uint32_t alpha1 = alpha * re;
          const uint32_t alpha2 = alpha * e;

          plot(x, y, rawColor, px == sx ? alpha1 : alpha);
          plot(x + 1, y, rawColor, px == pex ? alpha2 : alpha);
        } else {
          plot(x, y, rawColor, alpha);
          plot(x + 1, y, rawColor, alpha);
        }
      }

      plot(ax, py2 + 1, rawColor, palpha2);

      apy -= pm;
    }
  }

  void Line::drawHorizontalLine(const int sx, const int ex, const int y, const uint32_t thickness,
                                const uint32_t rawColor, const uint32_t alpha) {
    for(auto py = y; py < (y + thickness); ++py) {
      for (auto x = sx; x <= ex; ++x) {
        plot(x, py, rawColor, alpha);
      }
    }
  }

  void Line::drawVerticalLine(const int sy, const int ey, const int x, const uint32_t thickness,
                                const uint32_t rawColor, const uint32_t alpha) {
    for(auto px = x; px < (x + thickness); ++px) {
      for (auto y = sy; y <= ey; ++y) {
        plot(px, y, rawColor, alpha);
      }
    }
  }

  void Line::calculatePxPy(int& px, int& py, const int dx, const int dy, const int sx, const int sy, const uint32_t thickness) {
    px = sx;
    py = sy;
    if (thickness > 1) {
      float m = float(dx) / float(dy);
      float r = sqrt(1 + m * m);
      px = float(sx) + thickness / r;
      py = float(sy) + thickness * m / r;
    }
  }
}