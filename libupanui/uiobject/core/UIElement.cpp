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

namespace upanui {
  UIElement::UIElement(const int x, const int y, const uint32_t width, const uint32_t height)
    : UIObjectImpl(x, y, width, height), _drawBuffer(nullptr, 0, 0), _localBuffer(nullptr), _currentBoundaryCheckResult(Outside) {
  }

  UIElement::~UIElement() noexcept {
    delete _localBuffer;
  }

  int UIElement::drawX() const {
    return parent().drawX() + x();
  }

  int UIElement::drawY() const {
    return parent().drawY() + y();
  }

  void UIElement::positionChanged() {
    parent().redraw();
  }

  void UIElement::sizeChanged() {
    parent().redraw();
  }

  void UIElement::contentChanged() {
    redraw();
  }

  void UIElement::setupDrawBuffer(const UIObject::BoundaryCheckResult boundaryCheckResult) {
    if (boundaryCheckResult == BoundaryCheckResult::Outside) {
      if (_localBuffer) {
        delete _localBuffer;
        _localBuffer = nullptr;
      }
      _drawBuffer = FrameBuffer(nullptr, 0, 0);
    } else if (boundaryCheckResult == BoundaryCheckResult::Inside) {
      if (_localBuffer) {
        delete _localBuffer;
        _localBuffer = nullptr;
      }

      auto& parentDrawBuffer = parent().drawBuffer();
      const auto cx = x() + parent().borderThickness();
      const auto cy = y() + parent().borderThickness();
      FrameBufferInfo frameBufferInfo = parent().drawBuffer().frameBufferInfo();
      frameBufferInfo._frameBuffer = parentDrawBuffer.buffer() + cx + cy * parentDrawBuffer.width();
      _drawBuffer = FrameBuffer(frameBufferInfo);
    } else if (boundaryCheckResult == BoundaryCheckResult::PartiallyInside) {
      if (_localBuffer != nullptr && (_drawBuffer.width() != width() || _drawBuffer.height() != height())) {
        delete _localBuffer;
        _localBuffer = nullptr;
      }
      if (_localBuffer == nullptr) {
        _localBuffer = new uint32_t[width() * height()];
        _drawBuffer = FrameBuffer(_localBuffer, width(), height());
      }
    } else {
      throw upan::exception(XLOC, "Unsupported BoundaryCheckResult: %d", boundaryCheckResult);
    }
  }
}
