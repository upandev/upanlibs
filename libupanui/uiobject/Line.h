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

#pragma once

#include <UILeafElement.h>
#include <GCoreFunctions.h>

namespace upanui {
  class Line : public UILeafElement {
  public:
    int x1() const { return _x1; }
    int y1() const { return _y1; }
    int x2() const { return _x2; }
    int y2() const { return _y2; }
    uint32_t thickness() const { return _thickness; }

    void updateXY(const int x1, const int y1, const int x2, const int y2);
    void updateThickness(const uint32_t thickness);

  protected:
    virtual ~Line() {}
    Line(const int x1, const int y1, const int x2, const int y2, const uint32_t thickness);
    void doDraw() override;

  private:
    void plot(int x, int y, const uint32_t color, const uint32_t alpha);
    void drawLine(const uint32_t rawColor, const uint32_t alpha);
    void drawLineWithLowSlope(const uint32_t rawColor, const uint32_t alpha);
    void drawLineWithHighSlope(const uint32_t rawColor, const uint32_t alpha);
    void drawHorizontalLine(const uint32_t rawColor, const uint32_t alpha);
    void drawVerticalLine(const uint32_t rawColor, const uint32_t alpha);

    void updateLayoutArea();

    class Spec {
    public:
      void calculate(const int x1, const int y1, const int x2, const int y2, const uint32_t thickness);

      int sx() const { return _sx; }
      int sy() const { return _sy; }
      int ex() const { return _ex; }
      int ey() const { return _ey; }
      int psx() const { return _psx; }
      int psy() const { return _psy; }
      int pex() const { return _pex; }
      int pey() const { return _pey; }
      float m() const { return _m; }
      float pm() const { return _pm; }

    private:
      int _sx;
      int _sy;
      int _ex;
      int _ey;
      int _psx;
      int _psy;
      int _pex;
      int _pey;
      float _m;
      float _pm;
    };

  private:
    int _x1;
    int _y1;
    int _x2;
    int _y2;
    uint32_t _thickness;
    Spec _spec;
    GCoreFunctions::PixelCache _pixelCache;

    friend class UIObjectFactory;
  };
}