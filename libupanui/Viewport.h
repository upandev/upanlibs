/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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

namespace upanui {
  class Viewport {
  public:
    Viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) : _x(x), _y(y), _width(width), _height(height) {}

    uint32_t x() const {
      return _x;
    }
    void x(uint32_t val) {
      _x = val;
    }

    uint32_t y() const {
      return _y;
    }
    void y(uint32_t val) {
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
    uint32_t _x;
    uint32_t _y;
    uint32_t _width;
    uint32_t _height;
  };
}