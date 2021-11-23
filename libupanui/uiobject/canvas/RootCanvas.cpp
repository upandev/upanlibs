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

#include <RootCanvas.h>
#include <GraphicsContext.h>

namespace upanui {
  RootCanvas::RootCanvas(const int x, const int y, const uint32_t width, const uint32_t height)
    : Canvas(x, y, width, height), _bgColor(upan::option<uint32_t>::empty()) {
    gc().frame().updateViewport(x, y, width, height);
  }

  void RootCanvas::draw() {
    if (!_bgColor.isEmpty()) {
      gc().frame().fillRect(0, 0, width(), height(), _bgColor.value());
    }
    for(auto& child : children()) {
      child->draw();
    }
  }

  void RootCanvas::noBackgroundColor() {
    _bgColor = upan::option<uint32_t>::empty();
    contentChanged();
  }

  void RootCanvas::backgroundColor(const uint32_t color) {
    _bgColor = color;
    contentChanged();
  }
}