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
  class MouseData {
  public:
    MouseData(uint32_t x, uint32_t y, bool isMiddlePressed, bool isRightPressed, bool isLeftPressed)
    : _x(x), _y(y), _padding(0),
      _isMiddlePressed(isMiddlePressed), _isRightPressed(isRightPressed), _isLeftPressed(isLeftPressed) {
    }

    MouseData() : MouseData(0, 0, false, false, false) {
    }

    int x() const {
      return _x;
    }

    int y() const {
      return _y;
    }

    bool isMiddlePressed() const {
      return _isMiddlePressed;
    }

    bool isRightPressed() const {
      return _isRightPressed;
    }

    bool isLeftPressed() const {
      return _isLeftPressed;
    }

    bool anyButtonPressed() const {
      return _isMiddlePressed || _isRightPressed || _isLeftPressed;
    }

    bool operator==(const MouseData& r) const {
      return _x == r._x && _y == r._y && _isMiddlePressed == r._isMiddlePressed && _isRightPressed == r._isRightPressed && _isLeftPressed == r._isLeftPressed;
    }

  private:
    int16_t _x;
    int16_t _y;
    uint8_t _padding:5;
    uint8_t _isMiddlePressed:1;
    uint8_t _isRightPressed:1;
    uint8_t _isLeftPressed:1;
  } PACKED;
}
