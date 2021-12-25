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

namespace upanui {
  class GraphicsContext;
  class KeyboardEvent;
  class MouseEvent;

  class UIObjectImpl : public UIObject {
  protected:
    UIObjectImpl(const int x, const int y, const uint32_t width, const uint32_t height);

  public:
    int x() const override { return _x; }
    int y() const override { return _y; }
    uint32_t width() const override { return _width; }
    uint32_t height() const override { return _height; }
    uint32_t borderThickness() const override { return _borderThickness; }

    void x(const int) override;
    void y(const int) override;
    void width(const uint32_t) override;
    void height(const uint32_t) override;
    void borderThickness(const uint32_t) override;

    UIObject& parent() const override;
    const upan::set<UIObject*>& children() override;

    void add(UIObject& child) override;
    void remove() override;
    void redraw() override;

    GraphicsContext& gc() {
      return _gc;
    }

    upan::option<UIObject&> uiObjectUnderCursor(const int x, const int y) override;

  protected:
    void onKeyboardEvent(const KeyboardEvent& event) override {}
    void onMouseEvent(const MouseEvent& event) override {}
    void onMouseFocus() override {}
    void onLoseMouseFocus() override {}

    BoundaryCheckResult checkBoundary(UIObject& child) override;
    void drawChild(UIObject& child) override;

    bool inside(const int x, const int y) const;

  private:
    int _x;
    int _y;
    uint32_t _width;
    uint32_t _height;
    uint32_t _borderThickness;

    GraphicsContext& _gc;
  };
}
