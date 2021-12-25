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

#include <UIObject.h>
#include <KeyboardEventHandler.h>
#include <MouseEventHandler.h>
#include <GraphicsContext.h>

namespace upanui {
  UIObjectImpl::UIObjectImpl(const int x, const int y, const uint32_t width, const uint32_t height)
    : _x(x), _y(y), _width(width), _height(height), _borderThickness(0), _gc(GraphicsContext::Instance()) {
  }

  void UIObjectImpl::x(const int x) {
    if (_x != x) {
      _x = x;
      positionChanged();
    }
  }

  void UIObjectImpl::y(const int y) {
    if (_y != y) {
      _y = y;
      positionChanged();
    }
  }

  void UIObjectImpl::width(const uint32_t width) {
    if (_width != width) {
      _width = width;
      sizeChanged();
    }
  }

  void UIObjectImpl::height(const uint32_t height) {
    if (_height != height) {
      _height = height;
      sizeChanged();
    }
  }

  void UIObjectImpl::borderThickness(const uint32_t thickness) {
    if (_borderThickness != thickness) {
      _borderThickness = thickness;
      contentChanged();
    }
  }

  UIObject& UIObjectImpl::parent() const {
    return _gc.uiObjectManager().parent(*this);
  }

  const upan::set<UIObject*>& UIObjectImpl::children() {
    return _gc.uiObjectManager().children(*this);
  }

  void UIObjectImpl::add(UIObject& child) {
    _gc.uiObjectManager().add(*this, child);
  }

  void UIObjectImpl::remove() {
    _gc.uiObjectManager().remove(*this);
  }

  void UIObjectImpl::redraw() {
    _gc.uiObjectManager().queueForRedraw(*this);
  }

  bool UIObjectImpl::inside(const int x, const int y) const {
    const int objX = drawX();
    const int objY = drawY();
    return x >= objX && x < (int)(objX + _width) && y >= objY && y <= (int)(objY + _height);
  }

  upan::option<UIObject&> UIObjectImpl::uiObjectUnderCursor(const int x, const int y) {
    if (inside(x, y)) {
      for(auto& child : children()) {
        const upan::option<UIObject&> o = child->uiObjectUnderCursor(x, y);
        if (!o.isEmpty()) {
          return o;
        }
      }
      return upan::option<UIObject&>(this);
    } else {
      return upan::option<UIObject&>::empty();
    }
  }

  UIObject::BoundaryCheckResult UIObjectImpl::checkBoundary(UIObject& child) {
    const auto cx1 = child.x();
    const auto cy1 = child.y();
    const auto cx2 = cx1 + child.width() - 1;
    const auto cy2 = cy1 + child.height() - 1;

    const auto pwidth = width() - 2 * borderThickness();
    const auto pheight = height() - 2 * borderThickness();

    if (cx1 >= 0 && cx2 < pwidth && cy1 >= 0 && cy2 < pheight) {
      return BoundaryCheckResult::Inside;
    }

    if ((cx1 < 0 && cx2 < 0) ||
        (cy1 < 0 && cy2 < 0) ||
        (cx1 >= pwidth && cx2 >= pwidth) ||
        (cy1 >= pheight && cy2 >= pheight)) {
      return BoundaryCheckResult::Outside;
    }

    return BoundaryCheckResult::PartiallyInside;
  }

  void UIObjectImpl::drawChild(UIObject& child) {
    auto& childDrawBuffer = child.drawBuffer();
    auto& parentDrawBuffer = drawBuffer();

    const auto pwidth = width() - borderThickness();
    const auto pheight = height() - borderThickness();

    const int sx1 = child.x() >= 0 ? 0 : 0 - child.x();
    const int sy1 = child.y() >= 0 ? 0 : 0 - child.y();

    const int dx1 = (child.x() >= 0 ? child.x() : 0) + borderThickness();
    const int dy1 = (child.y() >= 0 ? child.y() : 0) + borderThickness();

    const int w = (child.width() - sx1) <= (pwidth - dx1) ? child.width() - sx1 : pwidth - dx1;
    const int h = (child.height() - sy1) <= (pheight - dy1) ? child.height() - sy1 : pheight - dy1;

    const int srcXOffset = sx1 * childDrawBuffer.bytesPerPixel();
    const int destXOffset = dx1 * parentDrawBuffer.bytesPerPixel();
    const int copyWidth = w * childDrawBuffer.bytesPerPixel();

    //printf("\n%d, %d, %d, %d, %d, %d, %d, %d, %d\n", srcX1, srcY1, destX1, destY1, w, h, srcXOffset, destXOffset, copyWidth);

    for(int y = 0; y < h; ++y) {
      int srcOffet = srcXOffset + (sy1 + y) * childDrawBuffer.pitch();
      int destOffset = destXOffset + (dy1 + y) * parentDrawBuffer.pitch();
      memcpy((void*)((uint32_t)parentDrawBuffer.buffer() + destOffset), (void*)((uint32_t)childDrawBuffer.buffer() + srcOffet), copyWidth);
    }
  }
}
