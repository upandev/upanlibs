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

#include <stdlib.h>
#include <stdio.h>
#include <usfncontext.h>
#include <option.h>
#include <set.h>
#include <list.h>

namespace upanui {
  class GraphicsContext;
  class KeyboardEvent;
  class MouseEvent;
  class DrawBuffer;
  class Layout;
  class MouseEventHandler;

  class UIObject {
  protected:
    UIObject() {}
    UIObject(const UIObject&) = delete;
    UIObject& operator=(const UIObject&) = delete;

  public:
    virtual int x() const = 0;
    virtual int y() const = 0;
    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
    virtual uint32_t backgroundColor() const = 0;
    virtual uint32_t backgroundColorForDraw() const = 0;
    virtual uint8_t  backgroundColorAlpha() const = 0;
    virtual uint32_t borderColor() const = 0;
    virtual uint8_t borderColorAlpha() const = 0;
    virtual uint32_t borderThickness() const = 0;

    virtual void x(const int) = 0;
    virtual void y(const int) = 0;
    virtual void xy(int, int) = 0;
    virtual void width(const uint32_t) = 0;
    virtual void height(const uint32_t) = 0;
    virtual void backgroundColor(const uint32_t color) = 0;
    virtual void backgroundColorAlpha(const uint8_t) = 0;
    virtual void borderColor(const uint32_t) = 0;
    virtual void borderColorAlpha(const uint8_t) = 0;
    virtual void borderThickness(const uint32_t) = 0;

    virtual int drawX() const = 0;
    virtual int drawY() const = 0;
    virtual void draw() = 0;
    virtual void drawTopDown() = 0;
    virtual void drawToTop() = 0;

    virtual void positionChanged() = 0;
    virtual void sizeChanged() = 0;
    virtual void contentChanged() = 0;

    virtual UIObject& parent() const = 0;
    virtual const upan::list<UIObject*>& children() = 0;

    virtual void add(UIObject& child) = 0;
    virtual void remove() = 0;
    virtual void redraw() = 0;

    virtual bool hasAlpha() = 0;
    virtual upan::option<UIObject&> uiObjectUnderCursor(const int x, const int y) = 0;

    virtual Layout& layout() = 0;
    virtual DrawBuffer& drawBuffer() = 0;

    virtual bool captureMouseEvents() const = 0;
    virtual void captureMouseEvents(bool) = 0;

    virtual void registerMouseEventHandler(MouseEventHandler& handler) = 0;
  protected:
    virtual ~UIObject() {}

    virtual void onKeyboardEvent(const KeyboardEvent& event) = 0;
    virtual void onMouseEvent(const MouseEvent& event) = 0;
    virtual void onMouseFocus() = 0;
    virtual void onLoseMouseFocus() = 0;

    friend class UIObjectManager;
  };
}
