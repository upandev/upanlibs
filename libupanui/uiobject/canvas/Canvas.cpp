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

#include <Canvas.h>
#include <GraphicsContext.h>
#include <Layout.h>

namespace upanui {
  Canvas::Canvas(const int x, const int y, const uint32_t width, const uint32_t height)
    : UIElement(x, y, width, height) {
  }

  void Canvas::draw() {
    if (backgroundColorAlpha() != 100) {
      parent().draw();
    } else {
      drawTopDown();

      if (!drawBuffer().isNull()) {
        parent().drawToTop();
      }
    }
  }

  void Canvas::drawToTop() {
    if (drawBuffer().isLocal()) {
      parent().layout().draw(*this);
    }

    if (!drawBuffer().isNull()) {
      parent().drawToTop();
    }
  }

  void Canvas::drawTopDown() {
    //1. do boundary check
    //2. if child is outside then no need to draw
    //3. if child is inside then child's buffer is same as parent's buffer
    //4. if child id partially-inside then create your separate buffer
    //5. Draw inside the buffer
    //6. Draw all children
    //7. if using parent buffer then end the draw
    //8. if using your own buffer then ask parent to copy the child buffer into it's buffer

    setupDrawBuffer();

    auto& drawBuf = drawBuffer();
    if (!drawBuf.isNull()) {
      layout().fill();
      doDraw(drawBuf);
      for (auto child: children()) {
        child->drawTopDown();
      }

      if (drawBuf.isLocal()) {
        parent().layout().draw(*this);
      }
    }
  }
}