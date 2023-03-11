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
  UIRoot::UIRoot(const int x, const int y, const uint32_t width, const uint32_t height)
  : UIObjectImpl(x, y, width, height),
    _layout(*this) {
    gc().frame().updateViewport(x, y, width, height);
    UIObjectImpl::drawBuffer().initLocal(gc().frame().frameBuffer());
  }

  void UIRoot::draw() {
    layout().fill();
    for(auto& child : children()) {
      child->drawTopDown();
    }
  }

  void UIRoot::drawTopDown() {
    throw upan::exception(XLOC, "unsupport drawTopDown because UIRoot can't be a child elemenet");
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


  void UIRoot::notifyChange(const ChangeState changeState) {
    if (isChangeNotificationLocked()) {
      return;
    }

    setChangeState(changeState);
    switch(changeState) {
      case Position:
        gc().frame().updateViewport(x(), y(), width(), height());
        break;
      case Size:
        gc().frame().updateViewport(x(), y(), width(), height());
        redraw();
        break;
      case Content:
        redraw();
        break;
    }
  }
}
