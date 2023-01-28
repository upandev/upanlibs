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

#include <RectangleCanvas.h>
#include <MouseEventHandler.h>

namespace upanui {
  class VerticalScroller : public RectangleCanvas {
  private:
    VerticalScroller(const int x, const int y,
                     const uint32_t width, const uint32_t height,
                     const uint32_t scrollBarWidth, const uint32_t scrollBarHeight);
    virtual ~VerticalScroller();

    void init();
    void add(UIObject& child) override;
    void handleMouseEvent(upanui::UIObject &uiObject, const upanui::MouseEvent &event);

    class ScrollerMouseHandler : public MouseEventHandler {
    public:
      explicit ScrollerMouseHandler(VerticalScroller &parent) : _parent(parent) {}
      void onEvent(upanui::UIObject &uiObject, const upanui::MouseEvent &event) override {
        _parent.handleMouseEvent(uiObject, event);
      }
    private:
      VerticalScroller& _parent;
    };

    friend class UIObjectFactory;

  private:
    uint32_t _scrollBarWidth;
    uint32_t _scrollBarHeight;

    ScrollerMouseHandler* _mouseHandler;
    //one child enforcer
    bool _childCheck;
    upan::option<UIObject&> _scrollableChild;
  };
}