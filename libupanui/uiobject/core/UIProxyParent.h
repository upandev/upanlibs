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

#include <UIObject.h>
#include "FrameBuffer.h"

namespace upanui {
  class UIProxyParent : public UIObject {
  private:
    UIProxyParent() {}

    int x() const override { return 0; };
    int y() const override { return 0; }
    int width() const override;
    int height() const override;
    uint32_t backgroundColor() const override { return 0; }
    uint32_t backgroundColorForDraw() const override { return 0; }
    uint8_t  backgroundColorAlpha() const override { return 0; }
    uint32_t borderColor() const override { return 0; }
    uint8_t  borderColorAlpha() const { return 0; }
    int borderThickness() const override { return 0; }

    void x(const int) override {}
    void y(const int) override {}
    void xy(int, int) override {}
    void height(int) override {}
    void backgroundColor(const uint32_t color) override {}
    void backgroundColorAlpha(const uint8_t) override {}
    void borderColor(const uint32_t) override {}
    void borderColorAlpha(const uint8_t) override {}
    void borderThickness(int) override {}

    int drawX() const override { return 0; }
    int drawY() const override { return 0; }
    void draw() override {
      throw upan::exception(XLOC, "draw is not applicable for UIProxyParent");
    }
    void drawTopDown() override {
      throw upan::exception(XLOC, "drawTopDown is not applicable for UIProxyParent");
    }
    void drawToTop() override {
      throw upan::exception(XLOC, "drawToTop is not applicable for UIProxyParent");
    }

    UIObject& parent() const override {
      throw upan::exception(XLOC, "No parent for UIProxyParent");
    }
    const upan::list<UIObject*>& children() override {
      throw upan::exception(XLOC, "No child relationship tracked for UIProxyParent");
    }

    void add(UIObject& child) override {
      throw upan::exception(XLOC, "Can't add  child to UIProxyParent");
    }

    void remove() override {}
    void redraw() override {}

    bool isRectangularShape() override {
      throw upan::exception(XLOC, "unsupported isRectangularShape for UIProxyParent");
    }

    bool hasAlphaLocal() override {
      throw upan::exception(XLOC, "unsupported hasAlphaLocal for UIProxyParent");
    }

    bool hasAlpha() override {
      throw upan::exception(XLOC, "unsupported hasAlpha for UIProxyParent");
    }

    bool captureMouseEvents() const override {
      throw upan::exception(XLOC, "unsupported (get)captureMouseEvents for UIProxyParent");
    }

    void captureMouseEvents(bool) override {
      throw upan::exception(XLOC, "unsupported (set)captureMouseEvents for UIProxyParent");
    }

    upan::option<UIObject&> uiObjectUnderCursor(int x, int y) override {
      throw upan::exception(XLOC, "unsupported uiObjectUnderCursor for UIProxyParent");
    }

    Layout& layout() override {
      throw upan::exception(XLOC, "unsupported layout for UIProxyParent");
    }

    DrawBuffer& drawBuffer() override {
      throw upan::exception(XLOC, "unsupported drawBuffer for UIProxyParent");
    }

    const DrawBuffer& drawBuffer() const override {
      throw upan::exception(XLOC, "unsupported drawBuffer for UIProxyParent");
    }

    void registerMouseEventHandler(MouseEventHandler& handler) override {
      throw upan::exception(XLOC, "unsupported event handlers for UIProxyParent");
    }

    int scrollY() const override { return -y(); }
    int scrollHeight() const override { return height(); }
    void vscroll(int rows, int scrollableHeight) override {}
    void hscroll(int columns) override {}
    void registerVerticalScroller(VerticalScroller&) override {}
    void removeVerticalScroller() override {}
    upan::option<VerticalScroller&> getVerticalScroller() override { return upan::option<VerticalScroller&>::empty(); }

    void notifyChange(const ChangeState changeState) override {}
    void setChangeState(const ChangeState) override {}
    bool isChangeState(const ChangeState, const bool only) const override { return false; }

  protected:
    void onKeyboardEvent(const KeyboardEvent& event) override {
      throw upan::exception(XLOC, "unsupported onKeyboardEvent for UIProxyParent");
    }
    void onMouseEvent(const MouseEvent& event) override {
      throw upan::exception(XLOC, "unsupported onMouseEvent for UIProxyParent");
    }
    void onMouseFocus() override {}
    void onLoseMouseFocus() override {}

    bool isHResizable() const override { return false; }
    bool isVResizable() const override { return false; }
    void setResizable(bool, bool) override {
      throw upan::exception(XLOC,"unsupported setResizable() for UIProxyParent");
    }

    bool width(int) override { return false; }
    void resize(ResizeMode, int, int) override {}
    HorizontalPlacementType getHorizontalPlacementType() const override { return HorizontalPlacementType::ABSOLUTE; }

    friend class UIObjectManager;
  };
}