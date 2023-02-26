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

#include <VerticalScroller.h>
#include <UIObjectFactory.h>
#include <IconButton.h>
#include <dtime.h>

namespace upanui {
  VerticalScroller::VerticalScroller(const int x, const int y,
                                     const uint32_t width, const uint32_t height,
                                     const uint32_t scrollBarWidth, const uint32_t scrollBarHeight)
    : RectangleCanvas(x, y, width, height),
      _scrollBarWidth(scrollBarWidth), _scrollBarHeight(scrollBarHeight),
      _mouseHandler(nullptr), _scrollUpBt(nullptr), _scrollDownBt(nullptr), _scrollBar(nullptr),
      _childCheck(false), _scrollableChild(upan::option<UIObject&>::empty()) {
  }

  VerticalScroller::~VerticalScroller() {
    delete _mouseHandler;
  }

  void VerticalScroller::init() {
    const int scrollerHeight = height() - _scrollBarWidth * 2;
    const int scrollBarX = width() - _scrollBarWidth;
    const int scrollBarTopBtY = 0;
    const int scrollerY = _scrollBarWidth;
    const int scrollBarBottomBtY = scrollerY + scrollerHeight;

    if (scrollerHeight > 0) {
      auto& scrollerCanvas = UIObjectFactory::createRectangleCanvas(*this, scrollBarX, scrollerY, _scrollBarWidth, scrollerHeight);
      scrollerCanvas.backgroundColor(0xFFFFFF);
    }

    _scrollUpBt = &UIObjectFactory::createIconButton(*this, PngImageResource::UP, scrollBarX, scrollBarTopBtY, _scrollBarWidth, _scrollBarWidth);
    _scrollUpBt->backgroundColor(0xFFFAABB);

    _scrollDownBt = &UIObjectFactory::createIconButton(*this, PngImageResource::DOWN, scrollBarX, scrollBarBottomBtY, _scrollBarWidth, _scrollBarWidth);
    _scrollDownBt->backgroundColor(0xFFFAABB);

    captureMouseEvents(true);

    _mouseHandler = new ScrollerMouseHandler(*this);
    _scrollUpBt->registerMouseEventHandler(*_mouseHandler);
    _scrollDownBt->registerMouseEventHandler(*_mouseHandler);

    _childCheck = true;
  }

  void VerticalScroller::add(UIObject& child) {
    if (_childCheck) {
      if (!_scrollableChild.isEmpty()) {
        throw upan::exception(XLOC, "Scroller can have only scrollable child");
      }
      _scrollableChild = upan::option<UIObject&>(child);
    }
    RectangleCanvas::add(child);
  }

  void VerticalScroller::handleMouseEvent(upanui::UIObject& sender, const upanui::MouseEvent& event) {
    const auto& e = event.getData();
    if (e.leftButtonState() == MouseData::State::PRESSED || e.leftButtonState() == MouseData::State::HOLD) {
      _scrollableChild.ifPresent([&](UIObject& child) {
        if (&sender == _scrollUpBt) {
          child.vscroll(-1, height());
        } else if (&sender == _scrollDownBt) {
          child.vscroll(1, height());
        }
      });
    }
  }
}