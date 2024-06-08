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

#include <UIRoot.h>
#include <GraphicsContext.h>
#include <GCoreFunctions.h>

namespace upanui {
  UIRoot::UIRoot(int x, int y, int width, int height)
          : UIObjectImpl(x, y, width, height, HorizontalPlacementType::STRETCHED),
            _layout(*this) {
    gc().frame().updateViewport(x, y, width, height);
    UIObjectImpl::drawBuffer().initLocal(gc().frame().frameBuffer());
  }

  void UIRoot::draw() {
    layout().fill();
    for (auto& child: children()) {
      child->drawTopDown();
    }
  }

  void UIRoot::drawTopDown() {
    throw upan::exception(XLOC, "unsupported drawTopDown() because UIRoot can't be a child element");
  }

  void UIRoot::drawToTop() {
    //no-op
  }

  int UIRoot::drawX() const {
    return 0;
  }

  int UIRoot::drawY() const {
    return 0;
  }

  UIObjectImpl::IntersectInfo UIRoot::intersect(int x, int y) const {
    const auto bgAlpha = backgroundColorAlpha();

    const int x1 = drawX();
    const int x2 = drawX() + width();
    const int y1 = drawY();
    const int y2 = drawY() + height();

    return {bgAlpha != 0, x - x1, x2 - x, y - y1, y2 - y };
  }

  void UIRoot::notifyChange(const ChangeState changeState) {
    if (isChangeNotificationLocked()) {
      return;
    }

    setChangeState(changeState);
    switch(changeState) {
      case ChangeState::Position:
        gc().frame().updateViewport(x(), y(), width(), height());
        break;
      case ChangeState::Size:
        //gc().frame().updateViewport(x(), y(), width(), height());
        redraw();
        break;
      case ChangeState::Content:
        redraw();
        break;
    }
  }

  void UIRoot::updateViewport() {
    GraphicsContext::Instance().frame().updateViewport(x(), y(), width(), height());
  }

  void UIRoot::resize(ResizeMode resizeMode, int dx, int dy) {
    ChangeNotificationLock cLock(*this);
    switch (resizeMode) {
      case ResizeMode::LEFT:
        if (!width(width() - dx)) {
          return;
        }
        x(x() + dx);
        break;

      case ResizeMode::RIGHT:
        if (!width(width() + dx)) {
          return;
        }
        break;
    }

    for(auto child = children().rbegin(); child != children().rend(); ++child) {
      child->resize(resizeMode, dx, dy);
    }

    redraw();
  }
}
