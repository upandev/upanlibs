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

#include <BaseFrame.h>
#include <string.h>

namespace upanui {
  BaseFrame::BaseFrame(const upanui::FrameBuffer& frameBuffer, const upanui::Viewport& viewport)
  : _frameBuffer(frameBuffer), _viewport(viewport) {
  }

  void BaseFrame::copy(const void *src, int len) {
    memcpy(_frameBuffer.buffer(), src, len);
    touch();
  }

  void BaseFrame::fillRect(uint32_t sx, uint32_t sy, uint32_t width, uint32_t height, uint32_t color) {
    uint32_t y_offset;
    bool changed = false;
    for(uint32_t y = sy; y < (sy + height) && y < _viewport.height(); ++y) {
      y_offset = y * _frameBuffer.pitch();
      for(uint32_t x = sx; x < (sx + width) && x < _viewport.width(); ++x) {
        auto p = (uint32_t*)((uint32_t)_frameBuffer.buffer() + y_offset + x * _frameBuffer.bytesPerPixel());
        *p = (color | 0xFF000000);
        changed = true;
      }
    }

    if (changed) {
      touch();
    }
  }

  bool BaseFrame::_updateViewport(int x, int y, uint32_t width, uint32_t height) {
    if (_viewport.x1() != x || _viewport.y1() != y || _viewport.width() != width || _viewport.height() != height) {
      _viewport.x1(x);
      _viewport.y1(y);
      _viewport.width(width);
      _viewport.height(height);
      return true;
    }
    return false;
  }
}