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

#include <stdlib.h>
#include <Image.h>
#include <uniq_ptr.h>

namespace upanui {
  class MouseCursor {
  public:
    MouseCursor();

    int realX() const { return _x; }
    int realY() const { return _y; }

    int drawX() const { return realX() + _xoffset; }
    int drawY() const { return realY() + _yoffset; }

    void x(int x) { _x = x; }
    void y(int y) { _y = y; }

    int width() const { return _image->width(); }
    int height() const { return _image->height(); }

    MouseCursorType type() const { return _type; }
    void type(MouseCursorType);

    const uint32_t* data() const {
      return const_cast<MouseCursor*>(this)->_image->data();
    }

  private:
    upan::uniq_ptr<Image> _pointerImg;
    upan::uniq_ptr<Image> _hresizerImg;
    upan::uniq_ptr<Image> _vresizerImg;
    upan::uniq_ptr<Image> _uhvresizerImg;
    upan::uniq_ptr<Image> _dhvresizerImg;
    const Image* _image;

    int _x;
    int _y;
    int _xoffset;
    int _yoffset;
    MouseCursorType _type;
  };
}