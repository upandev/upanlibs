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
#include <mosstd.h>
#include <algorithm.h>

namespace upanui {
  BaseFrame::BaseFrame(const upanui::FrameBuffer& frameBuffer, const upanui::Viewport& viewport)
  : _frameBuffer(frameBuffer), _viewport(viewport) {
  }

  void BaseFrame::fillRect(int sx, int sy, int width, int height, uint32_t color) {
    const int ex = upan::min(_viewport.width(), sx + width) - 1;
    const int ey = upan::min(_viewport.height(), sy + height) - 1;
    auto buffer = _frameBuffer.buffer();
    for(auto y = sy; y <= ey; ++y) {
      auto yOffset = y * _frameBuffer.width();
      for(auto x = sx; x <= ex; ++x) {
        buffer[x + yOffset] = color;
      }
    }
  }

  bool BaseFrame::_updateViewport(int x, int y, int width, int height) {
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