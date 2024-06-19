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
#include <GraphicsContext.h>

namespace upanui {
  VerticalScroller::VerticalScroller(int x, int y,
                                     int width, int height,
                                     int scrollBarWidth,
                                     HorizontalPlacementType horizontalPlacementType,
                                     VerticalPlacementType verticalPlacementType)
      : RectangleCanvas(x, y, width, height, horizontalPlacementType, verticalPlacementType),
        _scrollBarWidth(scrollBarWidth),
        _scrollBarMinY(scrollBarWidth),
        _scrollBarMaxY(height - scrollBarWidth),
        _scrollBarMaxHeight(height - scrollBarWidth * 2),
        _scrollMultiplier(8),
        _mouseHandler(nullptr), _scrollUpBt(nullptr), _scrollDownBt(nullptr), _scrollBar(nullptr),
        _childCheck(false), _scrollableChild(upan::option<UIObject &>::empty()) {
  }

  VerticalScroller::~VerticalScroller() {
  }

  void VerticalScroller::init() {
    if (height() <= 0 || width() <= _scrollBarWidth || _scrollBarWidth == 0) {
      throw upan::exception(XLOC, "Invalid vertical scroll bar dimensions");
    }

    _scrollerCanvas = &UIObjectFactory::createRectangleCanvas(*this, width() - _scrollBarWidth, 0,
                                                              _scrollBarWidth, height(),
                                                              HorizontalPlacementType::RIGHT_FIXED, VerticalPlacementType::STRETCHED);
    _scrollerCanvas->backgroundColor(0xd3d3d3);

    _scrollUpBt = &UIObjectFactory::createIconButton(*_scrollerCanvas, PngImageResource::UP, 0, 0,
                                                     _scrollBarWidth, _scrollBarWidth, HorizontalPlacementType::ABSOLUTE, VerticalPlacementType::TOP_FIXED);
    _scrollUpBt->backgroundColor(0xbebebe);

    _scrollDownBt = &UIObjectFactory::createIconButton(*_scrollerCanvas, PngImageResource::DOWN, 0, _scrollBarMaxY,
                                                       _scrollBarWidth, _scrollBarWidth, HorizontalPlacementType::ABSOLUTE, VerticalPlacementType::BOTTOM_FIXED);
    _scrollDownBt->backgroundColor(0xbebebe);

    _scrollBar = &UIObjectFactory::createRectangleCanvas(*_scrollerCanvas, 0, _scrollBarMinY,
                                                         _scrollBarWidth, _scrollBarMaxHeight, HorizontalPlacementType::ABSOLUTE, VerticalPlacementType::TOP_FIXED);
    //_scrollBar->borderThickness(1);
    //_scrollBar->borderColor(0x000000);
    _scrollBar->backgroundColor(0xa9a9a9);

    captureMouseEvents(true);
    _scrollerCanvas->captureMouseEvents(true);

    _mouseHandler.reset(new ScrollerMouseHandler(*this));
    _scrollUpBt->registerMouseEventHandler(*_mouseHandler);
    _scrollDownBt->registerMouseEventHandler(*_mouseHandler);
    _scrollBar->registerMouseEventHandler(*_mouseHandler);
    _scrollerCanvas->registerMouseEventHandler(*_mouseHandler);

    _childCheck = true;
  }

  bool VerticalScroller::applyHeightChange(int newHeight) {
    const int scrollBarMaxY = newHeight - _scrollBarWidth;
    if (scrollBarMaxY < _scrollBarWidth) {
      return false;
    }
    _scrollBarMaxY = scrollBarMaxY;

    const int scrollBarMaxHeight = newHeight - _scrollBarWidth * 2;
    if (scrollBarMaxHeight < _scrollBarWidth * 2) {
      return false;
    }
    _scrollBarMaxHeight = scrollBarMaxHeight;

    _scrollBar->height(_scrollBarMaxHeight);
    _scrollBar->y(_scrollBarMinY);
    return true;
  }

  void VerticalScroller::calibrateScrollbar() {
    const int minScrollBarHeight = _scrollBarWidth / 2;
    const int scrollBarMaxRunway = _scrollBarMaxHeight - minScrollBarHeight;
    const int scrollContentHeight = _scrollableChild.value().scrollHeight() >= height() ? _scrollableChild.value().scrollHeight() - height() : 0;
    _scrollMultiplier = 8;
    const int scrollBarRequiredRunway = scrollContentHeight / _scrollMultiplier;
//    while(scrollBarRequiredRunway > scrollBarMaxRunway) {
//      _scrollMultiplier <<= 1;
//      scrollBarRequiredRunway = scrollContentHeight / _scrollMultiplier;
//    }
    if (scrollBarRequiredRunway > scrollBarMaxRunway) {
      if (_scrollBarMaxHeight == _scrollBar->height()) {
        _scrollBar->height(minScrollBarHeight);
      }
      _scrollMultiplier = scrollContentHeight / (_scrollBarMaxHeight - _scrollBar->height());
    } else {
      const int scrollBarHeight = _scrollBarMaxHeight - scrollBarRequiredRunway;
      _scrollBar->height(scrollBarHeight);
    }

    updateScrollPosition(_scrollableChild.value().scrollY());
  }

  void VerticalScroller::add(UIObject &child) {
    if (_childCheck) {
      if (!_scrollableChild.isEmpty()) {
        throw upan::exception(XLOC, "Scroller can have only scrollable child");
      }
      _scrollableChild = upan::option<UIObject &>(child);
      child.registerVerticalScroller(*this);
      calibrateScrollbar();
    }
    RectangleCanvas::add(child);
  }

  void VerticalScroller::updateScrollPosition(int newY) {
    const int scrollNewY = newY / _scrollMultiplier;
    setScrollPosition(scrollNewY + _scrollBarMinY);
  }

  void VerticalScroller::setScrollPosition(int newY) {
    if (newY != _scrollBar->y()) {
      if (newY < _scrollBarMinY) {
        newY = _scrollBarMinY;
      } else if ((newY + _scrollBar->height()) >= _scrollBarMaxY) {
        newY = _scrollBarMaxY - _scrollBar->height();
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
          newY = _scrollBar->y() + event.getData().deltaY();
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
        } else if ((_scrollBar->y() + _scrollBar->height()) == _scrollBarMaxY) {
          int totalRows = child.scrollHeight() - child.scrollY();
          scrollRows = height() - totalRows;
        }
        child.vscroll(scrollRows, height());
      });
    } else {
      _scrollableChild.ifPresent([this](UIObject &child) {
        gc().setFocus(child);
      });
    }
  }
}