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
#include <MouseEventHandler.h>
#include <GCoreFunctions.h>

namespace upanui {
  UIRoot::UIRoot(const int x, const int y, const uint32_t width, const uint32_t height)
  : UIObjectImpl(x, y, width, height),
    _bgColor(0),
    _bgAlpha(100),
    _onDragHandler(upan::option<MouseEventHandler&>::empty()) {
    gc().frame().updateViewport(x, y, width, height);
  }

  void UIRoot::draw() {
    fill(drawX(), drawY(), 50 -1, height() - 1, _bgColor, _bgAlpha);
    fill(drawX() + 50, drawY(), width() - 1, height() - 1, _bgColor + 150, _bgAlpha);
    for(auto& child : children()) {
      child->drawTopDown();
    }
  }

  void UIRoot::drawTopDown() {
    throw upan::exception(XLOC, "unsupport drawTopDown because UIRoot can't be a child elemenet");
  }

  void UIRoot::drawToTop() {
    //no-op
  }

  void UIRoot::fill(int x1, int y1, int x2, int y2, uint32_t color, uint32_t alpha) {
    const auto& framebuffer = gc().frame().frameBuffer();
    color = (color & ~GCoreFunctions::ALPHA_MASK) | (alpha << 24);
    for(auto y = y1; y <= y2; ++y) {
      const auto yOffset = y * framebuffer.width();
      for(auto x = x1; x <= x2; ++x) {
        framebuffer.buffer()[x + yOffset] = color;
      }
    }
  }

  void UIRoot::backgroundColor(const uint32_t color) {
    _bgColor = color;
    contentChanged();
  }

  void UIRoot::backgroundColorAlpha(const uint8_t alpha) {
    if (alpha > 100) {
      throw upan::exception(XLOC, "alpha must be a value between 0 to 100");
    }
    _bgAlpha = alpha;
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

  FrameBuffer& UIRoot::drawBuffer() {
    return gc().frame().frameBuffer();
  }

  void UIRoot::onMouseEvent(const MouseEvent &event) {
    if (event.getData().leftButtonState() == MouseData::HOLD) {
      _onDragHandler.ifPresent([&](MouseEventHandler& handler) {
        handler.onEvent(*this, event);
      });
    }
  }
}
