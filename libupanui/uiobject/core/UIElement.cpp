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

#include <UIElement.h>
#include "GCoreFunctions.h"

namespace upanui {
  UIElement::UIElement(int x, int y, int32_t width, int32_t height, HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType)
    : UIObjectImpl(x, y, width, height, horizontalPlacementType, verticalPlacementType) {
  }

  UIElement::~UIElement() noexcept {
  }

  int UIElement::drawX() const {
    return parent().drawX() + x();
  }

  int UIElement::drawY() const {
    return parent().drawY() + y();
  }


  void UIElement::notifyChange(const ChangeState changeState) {
    if (isChangeNotificationLocked()) {
      return;
    }

    setChangeState(changeState);
    switch(changeState) {
      case ChangeState::Position:
        parent().setChangeState(ChangeState::Content);
        parent().redraw();
        break;
      case ChangeState::Size:
        parent().setChangeState(ChangeState::Content);
        parent().redraw();
        break;
      case ChangeState::Content:
        redraw();
        break;
    }
  }

  bool UIElement::skipRedraw() const {
    return drawBuffer().isLocal() && (isChangeState(ChangeState::Clean, true) || isChangeState(ChangeState::Position, true));
  }

  bool UIElement::setupDrawBuffer() {
    DrawBuffer& drawBuf = drawBuffer();
    if (needLocalDrawBuffer()) {
      return drawBuf.initLocal(width(), height());
    } else {
      const auto boundaryCheckResult = parent().layout().checkBoundary(*this);
      if (boundaryCheckResult == Layout::Outside) {
        drawBuf.clear();
        return true;
      } else if (boundaryCheckResult == Layout::Inside) {
        const auto cx = x() + parent().borderThickness();
        const auto cy = y() + parent().borderThickness();
        drawBuf.initFrom(parent().drawBuffer(), cx, cy, width(), height());
        return true;
      } else if (boundaryCheckResult == Layout::PartiallyInside) {
        return drawBuf.initLocal(width(), height());
      } else {
        throw upan::exception(XLOC, "Unsupported BoundaryCheckResult: %d", boundaryCheckResult);
      }
    }
  }

  bool UIElement::resizeLeft(int& dx) {
    switch (getHorizontalPlacementType()) {
      case HorizontalPlacementType::LEFT_FIXED:
        return false;

      case HorizontalPlacementType::ABSOLUTE:
      case HorizontalPlacementType::RIGHT_FIXED:
      case HorizontalPlacementType::RIGHT_STRETCHED:
        x(x() - dx);
        return false;

      case HorizontalPlacementType::LEFT_STRETCHED:
      case HorizontalPlacementType::STRETCHED:
        dx = width(width() - dx);
        if (!dx) {
          return false;
        }
        return true;
    }
    return false;
  }

  bool UIElement::resizeRight(int& dx) {
    switch (getHorizontalPlacementType()) {
      case HorizontalPlacementType::ABSOLUTE:
      case HorizontalPlacementType::LEFT_FIXED:
      case HorizontalPlacementType::LEFT_STRETCHED:
        return false;

      case HorizontalPlacementType::RIGHT_FIXED:
        x(x() + dx);
        return false;

      case HorizontalPlacementType::RIGHT_STRETCHED:
      case HorizontalPlacementType::STRETCHED:
        dx = -width(width() + dx);
        if (!dx) {
          return false;
        }
        return true;
    }
    return false;
  }

  bool UIElement::resizeTop(int& dy) {
    switch (getVerticalPlacementType()) {
      case VerticalPlacementType::TOP_FIXED:
        return false;

      case VerticalPlacementType::ABSOLUTE:
      case VerticalPlacementType::BOTTOM_FIXED:
      case VerticalPlacementType::BOTTOM_STRETCHED:
        y(y() - dy);
        return false;

      case VerticalPlacementType::TOP_STRETCHED:
      case VerticalPlacementType::STRETCHED:
        dy = height(height() - dy);
        if (!dy) {
          return false;
        }
        return true;
    }
    return false;
  }

  bool UIElement::resizeBottom(int& dy) {
    switch (getVerticalPlacementType()) {
      case VerticalPlacementType::ABSOLUTE:
      case VerticalPlacementType::TOP_FIXED:
      case VerticalPlacementType::TOP_STRETCHED:
        return false;

      case VerticalPlacementType::BOTTOM_FIXED:
        y(y() + dy);
        return false;

      case VerticalPlacementType::BOTTOM_STRETCHED:
      case VerticalPlacementType::STRETCHED:
        dy = -height(height() + dy);
        if (!dy) {
          return false;
        }
        return true;
    }
    return false;
  }
}
