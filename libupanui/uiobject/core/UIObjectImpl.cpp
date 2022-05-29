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
#include <MouseEventHandler.h>
#include <GraphicsContext.h>

namespace upanui {
  UIObjectImpl::UIObjectImpl(const int x, const int y, const uint32_t width, const uint32_t height)
    : _x(x), _y(y), _width(width), _height(height),
      _bgColor(0), _bgAlpha(100), _brColor(0xFFFFFF),
      _brAlpha(100), _borderThickness(0), _lockChangeNotification(false),
      _mouseEventHandler(upan::option<MouseEventHandler&>::empty()),
      _gc(GraphicsContext::Instance()) {
  }

  void UIObjectImpl::x(const int x) {
    if (_x != x) {
      _x = x;
      notifyChange(ChangeNotificationType::Position);
    }
  }

  void UIObjectImpl::y(const int y) {
    if (_y != y) {
      _y = y;
      notifyChange(ChangeNotificationType::Position);
    }
  }

  void UIObjectImpl::xy(const int x, const int y) {
    if (_y != y || _x != x) {
      _x = x;
      _y = y;
      notifyChange(ChangeNotificationType::Position);
    }
  }

  void UIObjectImpl::width(const uint32_t width) {
    if (_width != width) {
      _width = width;
      notifyChange(ChangeNotificationType::Size);
    }
  }

  void UIObjectImpl::height(const uint32_t height) {
    if (_height != height) {
      _height = height;
      notifyChange(ChangeNotificationType::Size);
    }
  }

  void UIObjectImpl::backgroundColor(const uint32_t color) {
    if (_bgColor != color) {
      _bgColor = color;
      onBackgroundColorChange();
      notifyChange(ChangeNotificationType::Content);
    }
  }

  void UIObjectImpl::backgroundColorAlpha(const uint8_t alpha) {
    if (_bgAlpha != alpha) {
      if (alpha > 100) {
        throw upan::exception(XLOC, "alpha must be a value between 0 to 100");
      }
      _bgAlpha = alpha;
      onBackgroundColorChange();
      notifyChange(ChangeNotificationType::Content);
    }
  }

  void UIObjectImpl::borderColor(const uint32_t color) {
    if (_brColor != color) {
      _brColor = color;
      notifyChange(ChangeNotificationType::Content);
    }
  }

  void UIObjectImpl::borderColorAlpha(const uint8_t alpha) {
    if (_brAlpha != alpha) {
      if (alpha > 100) {
        throw upan::exception(XLOC, "alpha must be a value between 0 to 100");
      }
      _brAlpha = alpha;
      notifyChange(ChangeNotificationType::Content);
    }
  }

  void UIObjectImpl::borderThickness(const uint32_t thickness) {
    if (_borderThickness != thickness) {
      _borderThickness = thickness;
      notifyChange(ChangeNotificationType::Content);
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
    redraw();
  }

  void UIObjectImpl::remove() {
    _gc.uiObjectManager().remove(*this);
    redraw();
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
      if (intersect(x, y)) {
        return upan::option<UIObject &>(this);
      }
    }

    return upan::option<UIObject&>::empty();
  }

  void UIObjectImpl::notifyChange(const ChangeNotificationType type) {
    if (_lockChangeNotification) {
      return;
    }

    switch(type) {
      case Position:
        positionChanged();
        break;
      case Size:
        sizeChanged();
        break;
      case Content:
        contentChanged();
        break;
    }
  }

  void UIObjectImpl::onMouseEvent(const MouseEvent &event) {
    _mouseEventHandler.ifPresent([&](MouseEventHandler& handler) {
      handler.onEvent(*this, event);
    });
  }
}
