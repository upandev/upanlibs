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

#include <UIObject.h>
#include <MouseEventHandler.h>
#include <GraphicsContext.h>
#include <GCoreFunctions.h>
#include <VerticalScroller.h>

namespace upanui {
  UIObjectImpl::UIObjectImpl(int x, int y,
                             int32_t width, int32_t height,
                             HorizontalPlacementType horizontalPlacementType,
                             VerticalPlacementType verticalPlacementType)
    : _zIndex(0), _x(x), _y(y), _width(width), _height(height),
      _bgColor(0), _bgAlpha(GCoreFunctions::MAX_ALPHA), _brColor(0xFFFFFF),
      _brAlpha(GCoreFunctions::MAX_ALPHA), _borderThickness(0), _lockChangeNotification(false),
      _mouseEventHandler(upan::option<MouseEventHandler&>::empty()), _captureMouseEvents(false),
      _verticalScroller(upan::option<VerticalScroller&>::empty()), _changeState((int)ChangeState::Content),
      _hResizable(false), _vResizable(false), _horizontalPlacementType(horizontalPlacementType), _verticalPlacementType(verticalPlacementType),
      _visible(true), _gc(GraphicsContext::Instance()) {

    if (_width > gc().frame().frameBuffer().width()) {
      _width = gc().frame().frameBuffer().width();
    }

    if (_width < MIN_OBJECT_SIZE) {
      _width = MIN_OBJECT_SIZE;
    }

    if (_height > gc().frame().frameBuffer().height()) {
      _height = gc().frame().frameBuffer().height();
    }

    if (_height < MIN_OBJECT_SIZE) {
      _height = MIN_OBJECT_SIZE;
    }

    if (_horizontalPlacementType == HorizontalPlacementType::ABSOLUTE
    || _horizontalPlacementType == HorizontalPlacementType::RIGHT_FIXED
    || _horizontalPlacementType == HorizontalPlacementType::LEFT_FIXED) {
      _minWidth = _width;
    } else {
      _minWidth = MIN_OBJECT_SIZE;
    }

    if (_verticalPlacementType == VerticalPlacementType::ABSOLUTE
    || _verticalPlacementType == VerticalPlacementType::TOP_FIXED
    || _verticalPlacementType == VerticalPlacementType::BOTTOM_FIXED) {
      _minHeight = _height;
    } else {
      _minHeight = MIN_OBJECT_SIZE;
    }

    _vWidth = _width;
    _vHeight = _height;
  }

  void UIObjectImpl::x(const int x) {
    if (_x != x) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _x = x;
      notifyChange(ChangeState::Position);
    }
  }

  void UIObjectImpl::y(const int y) {
    if (_y != y) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _y = y;
      notifyChange(ChangeState::Position);
    }
  }

  void UIObjectImpl::xy(const int x, const int y) {
    if (_y != y || _x != x) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _x = x;
      _y = y;
      notifyChange(ChangeState::Position);
    }
  }

  int UIObjectImpl::resizeWidth(int width, bool isPrimary) {
    if (width > gc().frame().frameBuffer().width()) {
      width = gc().frame().frameBuffer().width();
    }

    if (!isPrimary) {
      const int adx = width - _width;
      _vWidth += adx;

      if (adx >= 0) {
        if (_vWidth < minWidth()) {
          return 0;
        } else {
          width = _vWidth;
        }
      }
    }

    if (width < minWidth()) {
      width = minWidth();
    }

    const int dx = _width - width;
    if (dx != 0) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _width = width;
      if (isPrimary) _vWidth = width;
      notifyChange(ChangeState::Size);
    }

    return dx;
  }

  int UIObjectImpl::minWidth(int minWidth) {
    if (minWidth > _width) {
      minWidth = _width;
    }

    if (minWidth < MIN_OBJECT_SIZE) {
      minWidth = MIN_OBJECT_SIZE;
    }

    const int dx = _minWidth - minWidth;
    if (dx != 0) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _minWidth = minWidth;
    }

    return dx;
  }

  int UIObjectImpl::resizeHeight(int height, bool isPrimary) {
    if (height > gc().frame().frameBuffer().height()) {
      height = gc().frame().frameBuffer().height();
    }

    if (!isPrimary) {
      const int ady = height - _height;
      _vHeight += ady;

      if (ady >= 0) {
        if (_vHeight < minHeight()) {
          return 0;
        } else {
          height = _vHeight;
        }
      }
    }

    if (height < minHeight()) {
      height = minHeight();
    }

    const int dy = _height - height;
    if (dy != 0) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _height = height;
      if (isPrimary) _vHeight = height;
      notifyChange(ChangeState::Size);
    }

    return dy;
  }

  int UIObjectImpl::minHeight(int minHeight) {
    if (minHeight > _height) {
      minHeight = _height;
    }

    if (minHeight < MIN_OBJECT_SIZE) {
      minHeight = MIN_OBJECT_SIZE;
    }

    const int dy = _minHeight - minHeight;
    if (dy != 0) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _minHeight = minHeight;
    }

    return dy;
  }

  void UIObjectImpl::backgroundColor(const uint32_t color) {
    if (_bgColor != color) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _bgColor = color;
      onBackgroundColorChange();
      notifyChange(ChangeState::Content);
    }
  }

  void UIObjectImpl::backgroundColorAlpha(const uint8_t alpha) {
    if (_bgAlpha != alpha) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      if (alpha > GCoreFunctions::MAX_ALPHA) {
        throw upan::exception(XLOC, "alpha must be a value between 0 to %u", GCoreFunctions::MAX_ALPHA);
      }
      _bgAlpha = alpha;
      onBackgroundColorChange();
      notifyChange(ChangeState::Content);
      _gc.uiObjectManager().recalcHasAlpha();
    }
  }

  void UIObjectImpl::borderColor(const uint32_t color) {
    if (_brColor != color) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _brColor = color;
      notifyChange(ChangeState::Content);
    }
  }

  void UIObjectImpl::borderColorAlpha(const uint8_t alpha) {
    if (_brAlpha != alpha) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      if (alpha > GCoreFunctions::MAX_ALPHA) {
        throw upan::exception(XLOC, "alpha must be a value between 0 to %u", GCoreFunctions::MAX_ALPHA);
      }
      _brAlpha = alpha;
      notifyChange(ChangeState::Content);
      _gc.uiObjectManager().recalcHasAlpha();
    }
  }

  void UIObjectImpl::borderThickness(int thickness) {
    if (_borderThickness != thickness) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _borderThickness = thickness;
      notifyChange(ChangeState::Content);
      _gc.uiObjectManager().recalcHasAlpha();
    }
  }

  UIObject& UIObjectImpl::parent() const {
    return _gc.uiObjectManager().parent(*this);
  }

  const upan::list<UIObject*>& UIObjectImpl::children() {
    return _gc.uiObjectManager().children(*this);
  }

  void UIObjectImpl::add(UIObject& child) {
    upan::mutex_guard g(_gc.uiObjectManager().drawLock());
    _gc.uiObjectManager().add(*this, child);
    setChangeState(ChangeState::Content);
    child.setChangeState(ChangeState::Content);
    redraw();
  }

  void UIObjectImpl::remove() {
    upan::mutex_guard g(_gc.uiObjectManager().drawLock());
    _gc.uiObjectManager().remove(*this);
    setChangeState(ChangeState::Content);
    redraw();
  }

  void UIObjectImpl::redraw() {
    _gc.uiObjectManager().queueForRedraw(*this);
  }

  bool UIObjectImpl::isVisible() const {
    return parent().isVisible() ? getVisible() : false;
  }

  void UIObjectImpl::setVisible(bool visible) {
    if (visible != _visible) {
      upan::mutex_guard g(_gc.uiObjectManager().drawLock());
      _visible = visible;
      if (_visible) {
        notifyChange(ChangeState::Content);
      } else {
        notifyChange(ChangeState::Size);
      }
    }
  }

  bool UIObjectImpl::hasAlphaLocal() {
    if (backgroundColorAlpha() != GCoreFunctions::MAX_ALPHA)
      return true;

    if (borderThickness() > 0 && borderColorAlpha() != GCoreFunctions::MAX_ALPHA)
      return true;

    return false;
  }

  bool UIObjectImpl::hasAlpha() {
    if (hasAlphaLocal()) {
      return true;
    }

    for(auto& child : children()) {
      if (child->hasAlpha()) {
        return true;
      }
    }

    return false;
  }

  bool UIObjectImpl::inside(const int x, const int y) const {
    if (!isVisible()) {
      return false;
    }
    const int objX = drawX();
    const int objY = drawY();
    return x >= objX && x < (objX + _width) && y >= objY && y <= (objY + _height);
  }

  bool UIObjectImpl::activateResizer(const IntersectInfo& intersectInfo) {
    gc().setResizeMode(ResizeMode::NA);

    if (!intersectInfo._intersect) {
      return false;
    }

    if (!isHResizable() && !isVResizable()) {
      return false;
    }

    const bool lhresizer = intersectInfo._xLeftDelta >= 0 && intersectInfo._xLeftDelta < MIN_OBJECT_SIZE;
    const bool rhresizer = intersectInfo._xRightDelta >= 0 && intersectInfo._xRightDelta < MIN_OBJECT_SIZE;
    const bool tvresizer = intersectInfo._yTopDelta >= 0 && intersectInfo._yTopDelta < MIN_OBJECT_SIZE;
    const bool bvresizer = intersectInfo._yBottomDelta >= 0 && intersectInfo._yBottomDelta < MIN_OBJECT_SIZE;

    if (((lhresizer && tvresizer) || (rhresizer && bvresizer)) && isHResizable() && isVResizable()) {
      gc().setResizeMode((lhresizer && tvresizer) ? ResizeMode::LEFT_TOP : ResizeMode::RIGHT_BOTTOM);
    } else if (((lhresizer && bvresizer) || (rhresizer && tvresizer)) && isHResizable() && isVResizable()) {
      gc().setResizeMode((lhresizer && bvresizer) ? ResizeMode::LEFT_BOTTOM : ResizeMode::RIGHT_TOP);
    } else if ((lhresizer || rhresizer) && isHResizable()) {
      gc().setResizeMode(lhresizer ? ResizeMode::LEFT : ResizeMode::RIGHT);
    } else if ((tvresizer || bvresizer) && isVResizable()) {
      gc().setResizeMode(tvresizer ? ResizeMode::TOP : ResizeMode::BOTTOM);
    } else {
      return false;
    }
    return true;
  }

  upan::option<UIObject&> UIObjectImpl::uiObjectUnderCursor(const int x, const int y) {
    gc().setResizeMode(ResizeMode::NA);

    if (captureMouseEvents() && inside(x, y)) {
      const IntersectInfo intersectInfo = intersect(x, y);
      if (activateResizer(intersectInfo)) {
        return upan::option<UIObject &>(this);
      }

      for(auto child = children().rbegin(); child != children().rend(); ++child) {
        const upan::option<UIObject&> o = child->uiObjectUnderCursor(x, y);
        if (!o.isEmpty()) {
          return o;
        }
      }

      if (intersectInfo._intersect) {
        return upan::option<UIObject &>(this);
      }
    }
    return upan::option<UIObject&>::empty();
  }

  void UIObjectImpl::onMouseEvent(const MouseEvent &event) {
    _mouseEventHandler.ifPresent([&](MouseEventHandler& handler) {
      handler.onEvent(*this, event);
    });
  }

  void UIObjectImpl::vscroll(int rows, int scrollableHeight) {
    int newY = y() + rows;
    if (newY > 0) {
      newY = 0;
    } else {
      int h = scrollHeight() + newY;
      if (h < scrollableHeight) {
        newY = scrollableHeight - scrollHeight();
      }
    }
    y(newY);
  }

  void UIObjectImpl::hscroll(int columns) {
    x(x() + columns);
  }

  void UIObjectImpl::setChangeState(const ChangeState changeState) {
    if (changeState == ChangeState::Clean) {
      _changeState = (int)changeState;
    } else {
      _changeState |= (int)changeState;
    }
  }

  bool UIObjectImpl::isChangeState(ChangeState changeState, bool only) const {
    return (only || changeState == ChangeState::Clean) ? _changeState == (int)changeState : _changeState & (int)changeState;
  }

  void UIObjectImpl::resize(ResizeMode resizeMode, int dx, int dy, bool isPrimary) {
    ChangeNotificationLock cLock(*this);
    switch (resizeMode) {
      case ResizeMode::NA:
        return;

      case ResizeMode::LEFT: {
        dx = resizeLeft(dx, isPrimary);
        if (!dx) {
          return;
        }
      }
      break;

      case ResizeMode::RIGHT: {
        dx = resizeRight(dx, isPrimary);
        if (!dx) {
          return;
        }
      }
      break;

      case ResizeMode::TOP: {
        dy = resizeTop(dy, isPrimary);
        if (!dy) {
          return;
        }
      }
      break;

      case ResizeMode::BOTTOM: {
        dy = resizeBottom(dy, isPrimary);
        if (!dy) {
          return;
        }
      }
      break;

      case ResizeMode::LEFT_TOP: {
        dx = resizeLeft(dx, isPrimary);
        dy = resizeTop(dy, isPrimary);
        if (!dx && !dy) {
          return;
        }
      }
      break;

      case ResizeMode::LEFT_BOTTOM: {
        dx = resizeLeft(dx, isPrimary);
        dy = resizeBottom(dy, isPrimary);
        if (!dx && !dy) {
          return;
        }
      }
      break;

      case ResizeMode::RIGHT_TOP: {
        dx = resizeRight(dx, isPrimary);
        dy = resizeTop(dy, isPrimary);
        if (!dx && !dy) {
          return;
        }
      }
      break;

      case ResizeMode::RIGHT_BOTTOM: {
        dx = resizeRight(dx, isPrimary);
        dy = resizeBottom(dy, isPrimary);
        if (!dx && !dy) {
          return;
        }
      }
      break;
    }

    onResize();

    for(auto child = children().rbegin(); child != children().rend(); ++child) {
      child->resize(resizeMode, dx, dy, false);
    }

    if (isPrimary) {
      redraw();
    }
  }
}
