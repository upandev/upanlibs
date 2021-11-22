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

namespace upanui {
  class MouseCursor {
  public:
    MouseCursor(const uint32_t* imageBuffer, const int x, const int y, const uint32_t width, const uint32_t height)
    : _imageBuffer(imageBuffer), _x(x), _y(y), _width(width), _height(height) {
    }

    int x() const {
      return _x;
    }
    void x(const int x) {
      _x = x;
    }

    int y() const {
      return _y;
    }
    void y(const int y) {
      _y = y;
    }

    uint32_t width() const {
      return _width;
    }

    uint32_t height() const {
      return _height;
    }

    const uint32_t* data() const {
      return const_cast<MouseCursor*>(this)->_imageBuffer.get();
    }

  private:
    //assuming 4 bytes per pixel
    upan::uniq_ptr<const uint32_t> _imageBuffer;
    int _x;
    int _y;
    uint32_t _width;
    uint32_t _height;
  };
}