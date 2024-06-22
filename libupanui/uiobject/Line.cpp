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
#include <utility.h>

namespace upanui {
  Line::Line(int x1, int y1, int x2, int y2, int thickness, HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType)
      : UILeafElement(upan::min(x1, x2), upan::min(y1, y2), abs(x2 - x1), abs(y2 - y1), horizontalPlacementType, verticalPlacementType),
        _x1(x1), _y1(y1), _x2(x2), _y2(y2), _thickness(thickness) {
    updateLayoutArea();
  }

  void Line::doDraw() {
    drawBuffer().cleanBuffer();
    const auto alpha = backgroundColorAlpha();
    if (alpha == 0) {
      return;
    }

    if (_thickness == 0) {
      return;
    }

    const auto rawColor = backgroundColorForDraw() & GCoreFunctions::NO_ALPHA_MASK;
    drawLine(rawColor, backgroundColorAlpha());
  }

  void Line::drawLine(const uint32_t rawColor, const uint32_t alpha) {
    const int dy = abs(_spec.ey() - _spec.sy());
    const int dx = abs(_spec.ex() - _spec.sx());
    if (dy == 0) {
      drawHorizontalLine(rawColor, alpha);
    } else if (dx == 0) {
      drawVerticalLine(rawColor, alpha);
    } else if (dy < dx) {
      drawLineWithLowSlope(rawColor, alpha);
    } else {
      drawLineWithHighSlope(rawColor, alpha);
    }
  }

  void Line::plot(const int rx, const int ry, const uint32_t color, const uint32_t alpha) {
    auto& drawBuf = drawBuffer();
    const auto ax = rx - x();
    const auto ay = ry - y();
    if (ax < 0 || ay < 0 || ax >= drawBuf.width() || ay >= drawBuf.height()) {
      return;
    }
    GCoreFunctions::setPixel(drawBuf.at(ax, ay), color | (alpha << 24), _pixelCache, drawBuf.isLocal());
  }

  void Line::drawLineWithLowSlope(const uint32_t rawColor, const uint32_t alpha) {
    const bool positiveSlope = _spec.m() > 0;
    const int width = _spec.ex() - _spec.sx();

    float apx = _spec.sx();
    int px1 = _spec.sx();
    int prev_px1;

    for(int py = _spec.sy(); py <= _spec.psy(); ++py) {
      prev_px1 = px1;
      px1 = apx;
      const int px2 = px1 + width;

      const float pe = apx - px1;
      const float pre = 1 - pe;
      const uint32_t palpha1 = alpha * pre;
      const uint32_t palpha2 = alpha * pe;

      plot(px1, py, rawColor, palpha1);

      float ay = py;
      int prev_y;
      int y = py;
      for(int x = px1 + 1; x <= px2; ++x) {
        ay += _spec.m();
        prev_y = y;
        y = ay;
        //border line
        if (py == _spec.sy() || py == _spec.psy()) {
          const float e = ay - y;
          const float re = 1 - e;
          const uint32_t alpha1 = alpha * re;
          const uint32_t alpha2 = alpha * e;

          plot(x, y, rawColor, py == _spec.sy() ? alpha1 : alpha);
          if (py == _spec.psy()) {
            if (prev_y != y && prev_px1 != px1) {
              if (positiveSlope) {
                plot(x, prev_y, rawColor, py == _spec.sy() ? alpha1 : alpha);
              } else {
                plot(x - 1, y, rawColor, py == _spec.sy() ? alpha1 : alpha);
              }
            }
            plot(x, y + 1, rawColor, alpha2);
          }
        } else {
          plot(x, y, rawColor, alpha);
          if (prev_y != y && prev_px1 != px1) {
            if (positiveSlope) {
              plot(x, prev_y, rawColor, alpha);
            } else {
              plot(x - 1, y, rawColor, alpha);
            }
          }
        }
      }

      plot(px2 + 1, ay, rawColor, palpha2);

      apx += _spec.pm();
    }
  }

  void Line::drawLineWithHighSlope(const uint32_t rawColor, const uint32_t alpha) {
    const bool positiveSlope = _spec.m() > 0;
    const int height = _spec.ey() - _spec.sy();

    float apy = _spec.sy();
    int py1 = _spec.sy();
    int prev_py1;

    for(int px = _spec.sx(); px <= _spec.psx(); ++px) {
      prev_py1 = py1;
      py1 = apy;
      const int py2 = py1 + height;

      const float pe = apy - py1;
      const float rpe = 1 - pe;
      const uint32_t palpha1 = alpha * rpe;
      const uint32_t palpha2 = alpha * pe;

      plot(px, py1, rawColor, palpha1);

      float ax = px;
      int prev_x;
      int x = px;

      for(int y = py1 + 1; y <= py2; ++y) {
        ax += _spec.m();
        prev_x = x;
        x = ax;
        //border line
        if (px == _spec.sx() || px == _spec.psx()) {
          const float e = ax - x;
          const float re = 1 - e;
          const uint32_t alpha1 = alpha * re;
          const uint32_t alpha2 = alpha * e;

          plot(x, y, rawColor, px == _spec.sx() ? alpha1 : alpha);
          if (px == _spec.psx()) {
            if (prev_x != x && prev_py1 != py1) {
              if (positiveSlope) {
                plot(prev_x, y, rawColor, px == _spec.sx() ? alpha1 : alpha);
              } else {
                plot(x, y - 1, rawColor, px == _spec.sx() ? alpha1 : alpha);
              }
            }
            plot(x + 1, y, rawColor, alpha2);
          }
        } else {
          plot(x, y, rawColor, alpha);
          if (prev_x != x && prev_py1 != py1) {
            if (positiveSlope) {
              plot(prev_x, y, rawColor, alpha);
            } else {
              plot(x, y - 1, rawColor, alpha);
            }
          }
        }
      }

      plot(ax, py2 + 1, rawColor, palpha2);

      apy += _spec.pm();
    }
  }

  void Line::drawHorizontalLine(const uint32_t rawColor, const uint32_t alpha) {
    for(auto py = _spec.sy(); py < _spec.psy(); ++py) {
      for (auto x = _spec.sx(); x <= _spec.ex(); ++x) {
        plot(x, py, rawColor, alpha);
      }
    }
  }

  void Line::drawVerticalLine(const uint32_t rawColor, const uint32_t alpha) {
    for(auto px = _spec.sx(); px < _spec.psx(); ++px) {
      for (auto y = _spec.sy(); y <= _spec.ey(); ++y) {
        plot(px, y, rawColor, alpha);
      }
    }
  }

  void Line::updateXY(const int x1, const int y1, const int x2, const int y2) {
    _x1 = x1;
    _y1 = y1;
    _x2 = x2;
    _y2 = y2;
    updateLayoutArea();
    notifyChange(ChangeState::Size);
  }

  void Line::updateThickness(int thickness) {
    _thickness = thickness;
    updateLayoutArea();
    notifyChange(ChangeState::Content);
  }

  void Line::updateLayoutArea() {
    ChangeNotificationLock cLock(*this);
    _spec.calculate(_x1, _y1, _x2, _y2, _thickness);

    const int minX = upan::min(_spec.sx(), _spec.ex(), _spec.psx(), _spec.pex());
    const int maxX = upan::max(_spec.sx(), _spec.ex(), _spec.psx(), _spec.pex());
    const int minY = upan::min(_spec.sy(), _spec.ey(), _spec.psy(), _spec.pey());
    const int maxY = upan::max(_spec.sy(), _spec.ey(), _spec.psy(), _spec.pey());

    x(minX);
    y(minY);
    resizeWidth(maxX - minX + 1, true);
    resizeHeight(maxY - minY + 1, true);
  }

  void Line::Spec::calculate(const int x1, const int y1, const int x2, const int y2, int thickness) {
    const int ady = abs(y2 - y1);
    const int adx = abs(x2 - x1);

    const bool swapXY = (ady < adx) ? (x1 > x2) : (y1 > y2);
    if (swapXY) {
      _sx = x2; _sy = y2; _ex = x1; _ey = y1;
    } else {
      _sx = x1; _sy = y1; _ex = x2; _ey = y2;
    }

    const int dx = _ex - _sx;
    const int dy = _ey - _sy;

    _psx = _sx;
    _psy = _sy;
    if (thickness > 1) {
      if (dy == 0) { // horizontal line
        _psx = _sx;
        _psy = _sy + (x1 < x2 ? thickness : -thickness);
      } else if (dx == 0) { // vertical line
        _psx = _sx + (y1 > y2 ? thickness : -thickness);
        _psy = _sy;
      } else {
        const float m_inv = float(dx) / float(dy);
        const float r = sqrt(1 + m_inv * m_inv);

        const int ssign = m_inv < 0.0 ? -1 : 1;
        const int xsign = x1 < x2 ? -ssign : ssign;
        const int ysign = x1 < x2 ? 1 : -1;
        // Thickness is extended to the right of line from start-to-end as specified by original x & y
        // adjacent / hypotenuse = cos(theta) = 1 / sqrt(1 + tan^2(theta) = psx - sx / t = 1 / r = psx = sx + t / r
        // low slope --> px can be either to the left of or right of sx but py is always larger than sy
        // ex: 10 - 2.6 and 10 + 2.6 --> 8 and 12 --> so truncate float before adding to 10
        _psx = _sx + xsign * int(float(thickness) / r);
        _psy = _sy + ysign * int(float(thickness) * fabs(m_inv) / r);
      }
    }

    const int pdx = _psx - _sx;
    const int pdy = _psy - _sy;
    _pex = _ex + pdx;
    _pey = _ey + pdy;

    bool swapSP = false;
    if (dy == 0 || dx == 0) {
      _m = 0;
      _pm = 0;
      swapSP = _sx > _psx || _sy > _psy;
    } else if (dy < dx) {
      _m = float(dy) / float(dx);
      _pm = float(pdx) / float(pdy);
      swapSP = _sy > _psy;
    } else {
      _m = float(dx) / float(dy);
      _pm = float(pdy) / float(pdx);
      swapSP = _sx > _psx;
    }

    if (swapSP) {
      upan::swap(_sx, _psx);
      upan::swap(_sy, _psy);
      upan::swap(_ex, _pex);
      upan::swap(_ey, _pey);
    }
  }
}