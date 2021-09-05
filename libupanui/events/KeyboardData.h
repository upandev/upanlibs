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

#include <kb.h>

namespace upanui {
  class KeyboardData {
  public:
    KeyboardData(uint8_t ch,  bool isShiftPressed, bool isAltPressed, bool isCtrlPressed) :
    _ch(ch), _padding(0),
    _isShiftPressed(isShiftPressed), _isAltPressed(isAltPressed),
    _isCtrlPressed(isCtrlPressed) {
    }

    KeyboardData() : KeyboardData((uint8_t)Keyboard_NA_CHAR, false, false, false) {
    }

    uint8_t getCh() const {
      return _ch;
    }

    bool isAltPressed() const {
      return _isAltPressed;
    }

    bool isShiftPressed() const {
      return _isShiftPressed;
    }

    bool isCtrlPressed() const {
      return _isCtrlPressed;
    }

  private:
    uint8_t _ch;
    uint8_t _padding:5;
    uint8_t _isShiftPressed:1;
    uint8_t _isAltPressed:1;
    uint8_t _isCtrlPressed:1;
  } PACKED;
}
