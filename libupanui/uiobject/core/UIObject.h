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
#include <UIEnums.h>

namespace upanui {
  class GraphicsContext;
  class KeyboardEvent;
  class MouseEvent;
  class DrawBuffer;
  class Layout;
  class MouseEventHandler;
  class VerticalScroller;

  class UIObject {
  protected:
    UIObject() {}
    UIObject(const UIObject&) = delete;
    UIObject& operator=(const UIObject&) = delete;

  public:
    const static int MIN_OBJECT_SIZE = 5;

    virtual upan::string nameId() const = 0;
    virtual void nameId(const upan::string&) = 0;

    virtual int zIndex() const = 0;
    virtual void zIndex(int) = 0;

    virtual int x() const = 0;
    virtual int y() const = 0;
    virtual int width() const = 0;
    virtual int minWidth() const = 0;
    virtual int height() const = 0;
    virtual int minHeight() const = 0;
    virtual uint32_t backgroundColor() const = 0;
    virtual uint32_t backgroundColorForDraw() const = 0;
    virtual uint8_t  backgroundColorAlpha() const = 0;
    virtual uint32_t borderColor() const = 0;
    virtual uint8_t borderColorAlpha() const = 0;
    virtual int borderThickness() const = 0;

    virtual void x(int) = 0;
    virtual void y(int) = 0;
    virtual void xy(int, int) = 0;
    virtual void backgroundColor(const uint32_t color) = 0;
    virtual void backgroundColorAlpha(const uint8_t) = 0;
    virtual void borderColor(const uint32_t) = 0;
    virtual void borderColorAlpha(const uint8_t) = 0;
    virtual void borderThickness(int) = 0;

    virtual int drawX() const = 0;
    virtual int drawY() const = 0;
    virtual void draw() = 0;
    virtual void drawTopDown() = 0;
    virtual void drawToTop() = 0;

    virtual UIObject& parent() const = 0;
    virtual const upan::list<UIObject*>& children() = 0;

    virtual void add(UIObject& child) = 0;
    virtual void remove() = 0;
    virtual void redraw() = 0;

    virtual bool isRectangularShape() = 0;
    virtual bool hasAlphaLocal() = 0;
    virtual bool hasAlpha() = 0;
    virtual upan::option<UIObject&> uiObjectUnderCursor(int x, int y) = 0;

    virtual Layout& layout() = 0;
    virtual DrawBuffer& drawBuffer() = 0;
    virtual const DrawBuffer& drawBuffer() const = 0;

    virtual bool captureMouseEvents() const = 0;
    virtual void captureMouseEvents(bool) = 0;

    virtual void registerMouseEventHandler(MouseEventHandler& handler) = 0;

    virtual int scrollY() const = 0;
    virtual int scrollHeight() const = 0;
    virtual void vscroll(int rows, int scrollableHeight) = 0;
    virtual void hscroll(int columns) = 0;
    virtual upan::option<VerticalScroller&> getVerticalScroller() = 0;
    virtual void setVerticalScroller(VerticalScroller*) = 0;

    virtual void notifyChange(const ChangeState changeState) = 0;
    virtual void setChangeState(const ChangeState changeState) = 0;
    virtual bool isChangeState(const ChangeState changeState, const bool only) const = 0;

    virtual bool isHResizable() const = 0;
    virtual bool isVResizable() const = 0;
    virtual void setResizable(bool, bool) = 0;
    virtual void resize(ResizeMode resizeMode, int dx, int dy, bool isPrimary) = 0;
    virtual HorizontalPlacementType getHorizontalPlacementType() const = 0;
    virtual VerticalPlacementType getVerticalPlacementType() const = 0;

    virtual bool isVisible() const = 0;
    virtual void setVisible(bool) = 0;

  protected:
    virtual ~UIObject() {}

    virtual void onKeyboardEvent(const KeyboardEvent& event) = 0;
    virtual void onMouseEvent(const MouseEvent& event) = 0;
    virtual void onMouseFocus() = 0;
    virtual void onLoseMouseFocus() = 0;

    virtual int resizeWidth(int, bool isPrimary) = 0;
    virtual int minWidth(int) = 0;
    virtual int resizeHeight(int, bool isPrimary) = 0;
    virtual int minHeight(int) = 0;

    friend class UIObjectManager;
    //need this to adjust height of scroll-bar
    friend class VerticalScroller;
  };
}
