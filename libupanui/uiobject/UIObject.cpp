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
  UIObject::UIObject(const int x, const int y, const uint32_t width, const uint32_t height)
    : _x(x), _y(y), _width(width), _height(height), _gc(GraphicsContext::Instance()) {
  }

  void UIObject::x(const int x) {
    if (_x != x) {
      _x = x;
      positionChanged();
    }
  }

  void UIObject::y(const int y) {
    if (_y != y) {
      _y = y;
      positionChanged();
    }
  }

  void UIObject::width(const uint32_t width) {
    if (_width != width) {
      _width = width;
      sizeChanged();
    }
  }

  void UIObject::height(const uint32_t height) {
    if (_height != height) {
      _height = height;
      sizeChanged();
    }
  }

  upan::option<UIObject&> UIObject::parent() {
    return _gc.uiObjectManager().parent(*this);
  }

  const upan::set<UIObject*>& UIObject::children() {
    return _gc.uiObjectManager().children(*this);
  }

  void UIObject::add(UIObject& child) {
    _gc.uiObjectManager().add(*this, child);
  }

  void UIObject::remove() {
    _gc.uiObjectManager().remove(*this);
  }

  void UIObject::redraw() {
    _gc.uiObjectManager().queueForRedraw(*this);
  }

  void UIObject::positionChanged() {
    if (parent().isEmpty()) {
      _gc.frame().updateViewport(_x, _y, _width, _height);
    } else {
      parent().value().redraw();
    }
  }

  void UIObject::sizeChanged() {
    if (parent().isEmpty()) {
      _gc.frame().updateViewport(_x, _y, _width, _height);
      redraw();
    } else {
      parent().value().redraw();
    }
  }

  void UIObject::contentChanged() {
    redraw();
  }

  void UIObject::onKeyboardEvent(const KeyboardEvent& event) {
    for(auto handler : _keyboardEventHandlers) {
      handler->onEvent(*this, event);
    }
  }

  void UIObject::onMouseEvent(const MouseEvent& event) {
    for(auto handler : _mouseEventHandlers) {
      handler->onEvent(*this, event);
    }
  }
}
