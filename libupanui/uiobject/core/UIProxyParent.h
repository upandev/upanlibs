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

namespace upanui {
  class UIProxyParent : public UIObject {
  private:
    UIProxyParent() {}

    int x() const override { return 0; };
    int y() const override { return 0; }
    uint32_t width() const override;
    uint32_t height() const override;

    void x(const int) override {}
    void y(const int) override {}
    void width(const uint32_t) override {}
    void height(const uint32_t) override {}

    int drawX() const override { return 0; }
    int drawY() const override { return 0; }
    void draw() override {
      throw upan::exception(XLOC, "draw is not applicable for UIProxyParent");
    }

    void positionChanged() override {}
    void sizeChanged() override {}
    void contentChanged() override {}

    UIObject& parent() const override {
      throw upan::exception(XLOC, "No parent for UIProxyParent");
    }
    const upan::set<UIObject*>& children() override {
      throw upan::exception(XLOC, "No child relationship tracked for UIProxyParent");
    }

    void add(UIObject& child) override {
      throw upan::exception(XLOC, "Can't add  child to UIProxyParent");
    }

    void remove() override {}
    void redraw() override {}

    upan::option<UIObject&> uiObjectUnderCursor(const int x, const int y) override {
      throw upan::exception(XLOC, "unsupported uiObjectUnderCursor for UIProxyParent");
    }

  protected:
    void onKeyboardEvent(const KeyboardEvent& event) override {
      throw upan::exception(XLOC, "unsupported onKeyboardEvent for UIProxyParent");
    }
    void onMouseEvent(const MouseEvent& event) override {
      throw upan::exception(XLOC, "unsupported onMouseEvent for UIProxyParent");
    }
    void onMouseFocus() override {}
    void onLoseMouseFocus() override {}

    friend class UIObjectManager;
  };
}