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

#include <MouseCursor.h>
#include <ImageResource.h>

namespace upanui {
  MouseCursor::MouseCursor() : _x(0), _y(0), _xoffset(0), _yoffset(0), _type(MouseCursorType::NORMAL) {
    _pointerImg.reset(&upanui::PngImageResource::MOUSE_POINTER_CURSOR.create());
    _pointerImg->resize(12, 18);

    _hresizerImg.reset(&upanui::PngImageResource::MOUSE_HRESIZER_CURSOR.create());
    _hresizerImg->resize(32, 32);

    _vresizerImg.reset(&upanui::PngImageResource::MOUSE_VRESIZER_CURSOR.create());
    _vresizerImg->resize(32, 32);

    _uhvresizerImg.reset(&upanui::PngImageResource::MOUSE_UHVRESIZER_CURSOR.create());
    _uhvresizerImg->resize(32, 32);

    _dhvresizerImg.reset(&upanui::PngImageResource::MOUSE_DHVRESIZER_CURSOR.create());
    _dhvresizerImg->resize(32, 32);

    _image = _pointerImg.get();
  }

  void MouseCursor::type(MouseCursorType t) {
    if (t != _type) {
      switch (t) {
        case MouseCursorType::NORMAL:
          _image = _pointerImg.get();
          break;

        case MouseCursorType::HRESIZER:
          _image = _hresizerImg.get();
          break;

        case MouseCursorType::VRESIZER:
          _image = _vresizerImg.get();
          break;

        case MouseCursorType::UHVRESIZER:
          _image = _uhvresizerImg.get();
          break;

        case MouseCursorType::DHVRESIZER:
          _image = _dhvresizerImg.get();
          break;
      }
      _type = t;
      if (_type != MouseCursorType::NORMAL) {
        _xoffset = -(width() / 2);
        _yoffset = -(height() / 2);
      } else {
        _xoffset = 0;
        _yoffset = 0;
      }
    }
  }
}