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
    UIObjectImpl(int32_t x, int32_t y, int32_t width, int32_t height,
                 HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);

  public:
    const static int RESIZER_ZONE_LIMIT = 5;

    int x() const override { return _x; }
    int y() const override { return _y; }
    int width() const override { return _width; }
    int height() const override { return _height; }
    uint32_t backgroundColor() const override { return _bgColor; }
    uint32_t backgroundColorForDraw() const override { return _bgColor; }
    uint8_t  backgroundColorAlpha() const override { return _bgAlpha; }
    uint32_t borderColor() const override { return _brColor; }
    uint8_t  borderColorAlpha() const { return _brAlpha; }
    int borderThickness() const override { return _borderThickness; }

    void x(int) override;
    void y(int) override;
    void xy(int x, int y) override;
    void backgroundColor(const uint32_t color) override;
    void backgroundColorAlpha(const uint8_t) override;
    void borderColor(const uint32_t) override;
    void borderColorAlpha(const uint8_t) override;
    void borderThickness(int thickness) override;

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
    upan::option<UIObject&> uiObjectUnderCursor(int x, int y) override;

    bool captureMouseEvents() const override {
      return _captureMouseEvents;
    }

    void captureMouseEvents(bool val) override {
      _captureMouseEvents = val;
    }

    int scrollY() const override { return -y(); }
    int scrollHeight() const override { return height(); }
    void vscroll(int rows, int scrollableHeight) override;
    void hscroll(int columns) override;
    void registerVerticalScroller(VerticalScroller& verticalScroller) override;
    void removeVerticalScroller() override;
    upan::option<VerticalScroller&> getVerticalScroller() override { return _verticalScroller; }

    void setChangeState(const ChangeState) override;
    bool isChangeState(const ChangeState, const bool only) const override;

    bool isHResizable() const override { return _hResizable; }
    bool isVResizable() const override { return _vResizable; }
    void setResizable(bool hResizable, bool vResizable) override {
      _hResizable = hResizable;
      _vResizable = vResizable;
    }

    HorizontalPlacementType getHorizontalPlacementType() const override { return _horizontalPlacementType; }
    VerticalPlacementType getVerticalPlacementType() const override { return _verticalPlacementType; }

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
    typedef struct IntersectInfo {
      IntersectInfo() : _intersect(false), _xLeftDelta(0), _xRightDelta(0), _yTopDelta(0), _yBottomDelta(0) {}
      IntersectInfo(bool i, int xld, int xrd, int yld, int yrd) : _intersect(i), _xLeftDelta(xld), _xRightDelta(xrd), _yTopDelta(yld), _yBottomDelta(yrd) {}
      bool _intersect;
      int _xLeftDelta;
      int _xRightDelta;
      int _yTopDelta;
      int _yBottomDelta;
    } IntersectInfo;

    virtual IntersectInfo intersect(int x, int y) const {
      return { true, -1, -1, -1, -1 };
    }

    bool activateResizer(const IntersectInfo& intersectInfo);

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

    int width(int) override;
    int height(int) override;
    virtual bool applyHeightChange(int height) { return true; }

    virtual bool resizeLeft(int& dx) = 0;
    virtual bool resizeRight(int& dx) = 0;
    virtual bool resizeTop(int& dy) = 0;
    virtual bool resizeBottom(int& dy) = 0;
    void resize(ResizeMode resizeMode, int dx, int dy, bool allowRedraw) override;

  private:
    int _x;
    int _y;
    int _width;
    int _height;
    uint32_t _bgColor;
    uint8_t _bgAlpha;
    uint32_t _brColor;
    uint8_t _brAlpha;
    int _borderThickness;
    bool _lockChangeNotification;
    upan::option<MouseEventHandler&> _mouseEventHandler;
    bool _captureMouseEvents;
    upan::option<VerticalScroller&> _verticalScroller;
    int _changeState;
    DrawBuffer _drawBuffer;
    bool _hResizable;
    bool _vResizable;
    HorizontalPlacementType _horizontalPlacementType;
    VerticalPlacementType _verticalPlacementType;

    GraphicsContext& _gc;
  };
}
