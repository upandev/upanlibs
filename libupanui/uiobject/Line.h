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

namespace upanui {
  class Line : public UILeafElement {
  protected:
    virtual ~Line() {}
    Line(const int x1, const int y1, const int x2, const int y2);

    void doDraw() override;

  private:
    void plot(int x, int y, const uint32_t color, const uint32_t alpha);
    void drawLine(int x1, int y1, int x2, int y2, const uint32_t thickness,
                  const uint32_t rawColor, const uint32_t alpha);
    void drawLineWithLowSlope(int sx, int sy, int ex, int ey, uint32_t thickness,
                              const uint32_t rawColor, const uint32_t alpha);
    void drawLineWithHighSlope(int sx, int sy, int ex, int ey, uint32_t thickness,
                               const uint32_t rawColor, const uint32_t alpha);
    void drawHorizontalLine(const int sx, const int ex, const int y, const uint32_t thickness,
                            const uint32_t rawColor, const uint32_t alpha);
    void drawVerticalLine(const int sy, const int ey, const int x, const uint32_t thickness,
                            const uint32_t rawColor, const uint32_t alpha);
    void calculatePxPy(int& px, int& py, int dx, int dy, int sx, int sy, uint32_t thickness);

  private:
    const int _x1;
    const int _y1;
    const int _x2;
    const int _y2;
    friend class UIObjectFactory;
  };
}