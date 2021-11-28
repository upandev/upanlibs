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
    enum State { NORMAL, PRESSED, HOLD, RELEASED };

    MouseData(int x, int y, int deltaX, int deltaY, State leftButtonState, State rightButtonState, State middleButtonState)
      : _x(x), _y(y), _deltaX(deltaX), _deltaY(deltaY),
        _leftButtonState(leftButtonState), _rightButtonState(rightButtonState), _middleButtonState(middleButtonState), _padding(0) {
    }

    MouseData() : MouseData(0, 0, 0, 0, NORMAL, NORMAL, NORMAL) {
    }

    int x() const {
      return _x;
    }

    int y() const {
      return _y;
    }

    int deltaX() const {
      return _deltaX;
    }

    int deltaY() const {
      return _deltaY;
    }

    State leftButtonState() const {
      return static_cast<State>(_leftButtonState);
    }

    State rightButtonState() const {
      return static_cast<State>(_rightButtonState);
    }

    State middleButtonState() const {
      return static_cast<State>(_middleButtonState);
    }

    bool anyButtonPressed() const {
      return leftButtonState() == PRESSED || rightButtonState() == PRESSED || middleButtonState() == PRESSED;
    }

    bool anyButtonHeld() const {
      return leftButtonState() == HOLD || rightButtonState() == HOLD || middleButtonState() == HOLD;
    }

    bool operator==(const MouseData& r) const {
      return _x == r._x && _y == r._y
      && _leftButtonState == r._leftButtonState
      && _rightButtonState == r._rightButtonState
      && _middleButtonState == r._middleButtonState;
    }

    MouseData transition(int deltaX, int deltaY, bool leftPressed, bool rightPressed, bool middlePressed) {
      return {_x + deltaX, _y - deltaY, deltaX, deltaY,
        transitionState(leftButtonState(), leftPressed),
        transitionState(rightButtonState(), rightPressed),
        transitionState(middleButtonState(), middlePressed)};
    }

  private:

    static State transitionState(State state, bool pressed) {
      switch (state) {
        case NORMAL: return pressed ? PRESSED : NORMAL;
        case PRESSED:
        case HOLD: return pressed ? HOLD : RELEASED;
        case RELEASED: return pressed ? PRESSED : NORMAL;
        default: return NORMAL;
      }
    }

  private:
    int16_t _x;
    int16_t _y;
    int16_t _deltaX;
    int16_t _deltaY;
    uint8_t _leftButtonState:2;
    uint8_t _rightButtonState:2;
    uint8_t _middleButtonState:2;
    uint8_t _padding:2;
  } PACKED;
}
