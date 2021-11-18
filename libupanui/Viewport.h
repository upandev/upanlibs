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

#include <cdisplay.h>

namespace upanui {
  class Viewport {
  public:
    Viewport(int x, int y, uint32_t width, uint32_t height) : _x(x), _y(y), _width(width), _height(height) {}
    Viewport(const ViewportInfo& viewportInfo) : _x(viewportInfo._x), _y(viewportInfo._y), _width(viewportInfo._width), _height(viewportInfo._height) {}

    int x1() const {
      return _x;
    }

    int x2() const {
      return _x + _width - 1;
    }

    int y1() const {
      return _y;
    }

    int y2() const {
      return _y + _height - 1;
    }

    void x1(uint32_t val) {
      _x = val;
    }

    void y1(uint32_t val) {
      _y = val;
    }

    uint32_t width() const {
      return _width;
    }
    void width(uint32_t width) {
      _width = width;
    }

    uint32_t height() const {
      return _height;
    }
    void height(uint32_t height) {
      _height = height;
    }

  private:
    int _x;
    int _y;
    uint32_t _width;
    uint32_t _height;
  };
}