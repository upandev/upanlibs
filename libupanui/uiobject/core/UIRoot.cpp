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

#include <UIRoot.h>
#include <GraphicsContext.h>
#include <Button.h>
#include <Rectangle.h>

namespace upanui {
  UIRoot::UIRoot(const int x, const int y, const uint32_t width, const uint32_t height)
  : UIObjectImpl(x, y, width, height), _bgColor(upan::option<uint32_t>::empty()) {
    gc().frame().updateViewport(x, y, width, height);
  }

  void UIRoot::draw() {
    if (!_bgColor.isEmpty()) {
      Rectangle::fill(gc().frame().frameBuffer(), drawX(), drawY(), 50 -1, height() - 1, _bgColor.value());
      Rectangle::fill(gc().frame().frameBuffer(), drawX() + 50, drawY(), width() - 1, height() - 1, _bgColor.value() + 150);
    }
    for(auto& child : children()) {
      child->draw();
    }
  }

  void UIRoot::noBackgroundColor() {
    _bgColor = upan::option<uint32_t>::empty();
    contentChanged();
  }

  void UIRoot::backgroundColor(const uint32_t color) {
    _bgColor = color;
    contentChanged();
  }

  int UIRoot::drawX() const {
    return 0;
  }

  int UIRoot::drawY() const {
    return 0;
  }

  void UIRoot::positionChanged() {
    gc().frame().updateViewport(x(), y(), width(), height());
  }

  void UIRoot::sizeChanged() {
    gc().frame().updateViewport(x(), y(), width(), height());
    redraw();
  }

  void UIRoot::contentChanged() {
    redraw();
  }
}
