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
                                     const uint32_t scrollBarWidth)
      : RectangleCanvas(x, y, width, height),
        _scrollBarWidth(scrollBarWidth),
        _scrollBarMinY(scrollBarWidth),
        _scrollBarMaxY(height - scrollBarWidth),
        _scrollBarMaxHeight(height - scrollBarWidth * 2),
        _scrollMultiplier(8),
        _mouseHandler(nullptr), _scrollUpBt(nullptr), _scrollDownBt(nullptr), _scrollBar(nullptr),
        _childCheck(false), _scrollableChild(upan::option<UIObject &>::empty()) {
  }

  VerticalScroller::~VerticalScroller() {
    delete _mouseHandler;
  }

  void VerticalScroller::init() {
    if (height() <= 0 || width() <= _scrollBarWidth || _scrollBarWidth == 0) {
      throw upan::exception(XLOC, "Invalid vertical scroll bar dimensions");
    }

    _scrollerCanvas = &UIObjectFactory::createRectangleCanvas(*this, width() - _scrollBarWidth, 0, _scrollBarWidth, height());
    _scrollerCanvas->backgroundColor(0xFFFFFF);

    _scrollUpBt = &UIObjectFactory::createIconButton(*_scrollerCanvas, PngImageResource::UP, 0, 0, _scrollBarWidth, _scrollBarWidth);
    _scrollUpBt->backgroundColor(0xFFFAABB);

    _scrollDownBt = &UIObjectFactory::createIconButton(*_scrollerCanvas, PngImageResource::DOWN, 0, _scrollBarMaxY, _scrollBarWidth, _scrollBarWidth);
    _scrollDownBt->backgroundColor(0xFFFAABB);

    _scrollBar = &UIObjectFactory::createRectangleCanvas(*_scrollerCanvas, 0, _scrollBarMinY, _scrollBarWidth, _scrollBarWidth);
    _scrollBar->borderThickness(1);
    _scrollBar->borderColor(0x000000);
    _scrollBar->backgroundColor(0xFAD7A0);

    captureMouseEvents(true);
    _scrollerCanvas->captureMouseEvents(true);

    _mouseHandler = new ScrollerMouseHandler(*this);
    _scrollUpBt->registerMouseEventHandler(*_mouseHandler);
    _scrollDownBt->registerMouseEventHandler(*_mouseHandler);
    _scrollBar->registerMouseEventHandler(*_mouseHandler);
    _scrollerCanvas->registerMouseEventHandler(*_mouseHandler);

    _childCheck = true;
  }

  void VerticalScroller::caliberateScrollbar(bool directionUp) {
    const int minScrollBarHeight = _scrollBarWidth / 2;
    const int scrollBarMaxRunway = _scrollBarMaxHeight - minScrollBarHeight;
    const int scrollContentHeight = _scrollableChild.value().scrollHeight() >= (int)height() ? (int)_scrollableChild.value().scrollHeight() - (int)height() : 0;
    _scrollMultiplier = 8;
    int scrollBarRequiredRunway = scrollContentHeight / _scrollMultiplier;
    while(scrollBarRequiredRunway > scrollBarMaxRunway) {
      _scrollMultiplier <<= 1;
      scrollBarRequiredRunway = scrollContentHeight / _scrollMultiplier;
    }
    const int scrollBarHeight = _scrollBarMaxHeight - scrollBarRequiredRunway;
    _scrollBar->height(scrollBarHeight);
    updateScrollPosition(_scrollableChild.value().scrollY(), directionUp);
  }

  void VerticalScroller::add(UIObject &child) {
    if (_childCheck) {
      if (!_scrollableChild.isEmpty()) {
        throw upan::exception(XLOC, "Scroller can have only scrollable child");
      }
      _scrollableChild = upan::option<UIObject &>(child);
      child.registerVerticalScroller(*this);
      caliberateScrollbar(true);
    }
    RectangleCanvas::add(child);
  }

  void VerticalScroller::updateScrollPosition(int newY, bool directionUp) {
    const int scrollNewY = newY / _scrollMultiplier + ((newY % _scrollMultiplier) == 0 ? 0 : directionUp ? -1 : 1);
    setScrollPosition(scrollNewY + _scrollBarMinY);
  }

  void VerticalScroller::setScrollPosition(int newY) {
    if (newY != _scrollBar->y()) {
      if (newY < _scrollBarMinY) {
        newY = _scrollBarMinY;
      } else if (int(newY + _scrollBar->height()) >= _scrollBarMaxY) {
        newY = _scrollBarMaxY - (int)_scrollBar->height();
      }
      _scrollBar->y(newY);
    }
  }

  void VerticalScroller::handleMouseEvent(upanui::UIObject &sender, const upanui::MouseEvent &event) {
    const auto &e = event.getData();
    if (e.leftButtonState() == MouseData::State::PRESSED || e.leftButtonState() == MouseData::State::HOLD) {
      int newY = _scrollBar->y();
      if (&sender == _scrollUpBt) {
        newY -= 1;
      } else if (&sender == _scrollDownBt) {
        newY += 1;
      } else if (&sender == _scrollBar) {
        const int mouseViewY = event.viewY();
        if (mouseViewY >= int(_scrollUpBt->drawY() + _scrollBarWidth) && mouseViewY < _scrollDownBt->drawY()) {
          newY = _scrollBar->y() - event.getData().deltaY();
        }
      } else if (&sender == _scrollerCanvas && e.leftButtonState() == MouseData::State::PRESSED) {
        newY = event.viewY() - _scrollerCanvas->drawY();
        const int bottomY = _scrollBar->y() + _scrollBar->height();
        if (newY > bottomY) {
          newY = _scrollBar->y() + (newY - bottomY);
        }
      }
      const int oldScrollY = _scrollBar->y();
      setScrollPosition(newY);

      _scrollableChild.ifPresent([&](UIObject& child) {
        int scrollRows = (oldScrollY - _scrollBar->y()) * _scrollMultiplier;

        if (_scrollBar->y() == _scrollBarMinY) {
          scrollRows = child.scrollY() - 0;
        } else if (int(_scrollBar->y() + _scrollBar->height()) == _scrollBarMaxY) {
          int totalRows = (int)child.scrollHeight() - child.scrollY();
          scrollRows = (int)height() - totalRows;
        }
        child.vscroll(scrollRows, (int)height());
      });
    }
  }
}