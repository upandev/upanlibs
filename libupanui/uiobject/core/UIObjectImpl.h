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
#include <UIObject.h>
#include <DrawBuffer.h>
#include <GCoreFunctions.h>

namespace upanui {
  class GraphicsContext;
  class KeyboardEvent;
  class MouseEvent;
  class MouseEventHandler;
  class VerticalScroller;

  class UIObjectImpl : public UIObject {
  protected:
    UIObjectImpl(const int x, const int y, const uint32_t width, const uint32_t height);

  public:
    int x() const override { return _x; }
    int y() const override { return _y; }
    uint32_t width() const override { return _width; }
    uint32_t height() const override { return _height; }
    uint32_t backgroundColor() const override { return _bgColor; }
    uint32_t backgroundColorForDraw() const override { return _bgColor; }
    uint8_t  backgroundColorAlpha() const override { return _bgAlpha; }
    uint32_t borderColor() const override { return _brColor; }
    uint8_t  borderColorAlpha() const { return _brAlpha; }
    uint32_t borderThickness() const override { return _borderThickness; }

    void x(const int) override;
    void y(const int) override;
    void xy(const int x, const int y) override;
    void width(const uint32_t) override;
    void height(const uint32_t) override;
    void backgroundColor(const uint32_t color) override;
    void backgroundColorAlpha(const uint8_t) override;
    void borderColor(const uint32_t) override;
    void borderColorAlpha(const uint8_t) override;
    void borderThickness(const uint32_t) override;

    UIObject& parent() const override;
    const upan::list<UIObject*>& children() override;

    void add(UIObject& child) override;
    void remove() override;
    void redraw() override;

    GraphicsContext& gc() {
      return _gc;
    }

    void registerMouseEventHandler(MouseEventHandler& handler) override {
      _mouseEventHandler = upan::option<MouseEventHandler&>(handler);
      captureMouseEvents(true);
    }

    bool hasAlphaLocal() override;
    bool hasAlpha() override;
    upan::option<UIObject&> uiObjectUnderCursor(const int x, const int y) override;

    bool captureMouseEvents() const override {
      return _captureMouseEvents;
    }

    void captureMouseEvents(bool val) override {
      _captureMouseEvents = val;
    }

    int scrollY() const override { return -y(); }
    uint32_t scrollHeight() const override { return height(); }
    void vscroll(int rows, int scrollableHeight) override;
    void hscroll(int columns) override;
    void registerVerticalScroller(VerticalScroller& verticalScroller) override;
    void removeVerticalScroller() override;
    upan::option<VerticalScroller&> getVerticalScroller() override { return _verticalScroller; }

    void setChangeState(const ChangeState) override;
    bool isChangeState(const ChangeState, const bool only) const override;

  protected:
    uint32_t backgroundColorWithAlpha() const {
      return (_bgColor & GCoreFunctions::NO_ALPHA_MASK) | (_bgAlpha << 24);
    }
    virtual void onBackgroundColorChange() {}
    void onKeyboardEvent(const KeyboardEvent& event) override {}
    void onMouseEvent(const MouseEvent& event) override;
    void onMouseFocus() override {}
    void onLoseMouseFocus() override {}

    bool inside(int x, int y) const;
    virtual bool intersect(int x, int y) const {
      return true;
    }

    class ChangeNotificationLock {
    public:
      ChangeNotificationLock(UIObjectImpl& object) : _object(object) {
        _object._lockChangeNotification = true;
      }
      ~ChangeNotificationLock() {
        _object._lockChangeNotification = false;
      }
    private:
      UIObjectImpl& _object;
    };

    bool isChangeNotificationLocked() { return _lockChangeNotification; }

    DrawBuffer& drawBuffer() override {
      return _drawBuffer;
    }
    const DrawBuffer& drawBuffer() const override {
      return _drawBuffer;
    }

  private:
    int _x;
    int _y;
    uint32_t _width;
    uint32_t _height;
    uint32_t _bgColor;
    uint8_t _bgAlpha;
    uint32_t _brColor;
    uint8_t _brAlpha;
    uint32_t _borderThickness;
    bool _lockChangeNotification;
    upan::option<MouseEventHandler&> _mouseEventHandler;
    bool _captureMouseEvents;
    upan::option<VerticalScroller&> _verticalScroller;
    uint32_t _changeState;
    DrawBuffer _drawBuffer;

    GraphicsContext& _gc;
  };
}
