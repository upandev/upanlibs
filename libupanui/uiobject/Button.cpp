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

#include <Button.h>
#include <MouseEventHandler.h>

namespace upanui {
  Button::Button(const int x, const int y, const uint32_t width, const uint32_t height) : Canvas(x, y, width, height), _hover(false) {
    Button::onBackgroundColorChange();
  }

  void Button::onBackgroundColorChange() {
    const static auto lightness = 10u;
    auto bgColor = backgroundColor();

    auto b = bgColor & 0xFF;
    b += upan::min(0xFF - b, lightness);

    auto g = (bgColor & 0xFF00) >> 8;
    g += upan::min(0xFF - g, lightness);

    auto r = (bgColor & 0xFF0000) >> 16;
    r += upan::min(0xFF - r, lightness);

    _hoverColor = (bgColor & 0xFF000000) | r << 16 | g << 8 | b;
  }

  uint32_t Button::backgroundColorForDraw() {
    return _hover ? _hoverColor : backgroundColor();
  }

  void Button::onKeyboardEvent(const KeyboardEvent& event) {
  }

  void Button::onMouseEvent(const MouseEvent& event) {
  }

  void Button::onMouseFocus() {
    _hover = true;
    contentChanged();
  }

  void Button::onLoseMouseFocus() {
    _hover = false;
    contentChanged();
  }
}