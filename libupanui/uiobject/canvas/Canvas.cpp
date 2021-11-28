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
#include <Rectangle.h>

namespace upanui {
  Canvas::Canvas(const int x, const int y, const uint32_t width, const uint32_t height) : UIElement(x, y, width, height), _bgColor(0) {
  }

  void Canvas::backgroundColor(const uint32_t color) {
    _bgColor = color;
    contentChanged();
  }

  void Canvas::draw() {
    int dx1 = drawX();
    int dx2 = dx1 + width() - 1;

    int dy1 = drawY();
    int dy2 = dy1 + height() - 1;

    int bx1 = upan::max(0, parent().drawX());
    int bx2 = upan::min(gc().frame().viewport().width(), parent().drawX() +parent().width()) - 1;

    int by1 = upan::max(0, parent().drawY());
    int by2 = upan::min(gc().frame().viewport().height(), parent().drawY() + parent().height()) - 1;

    // UI element is not in visible area
    if ((dx2 < bx1) || (dy2 < by1) || (dx1 > bx2) || (dy1 > dy2)) {
      return;
    }

    // UI element is within the visible area
    if (dx1 >= bx1 && dx2 <= bx2 && dy1 >= by1 && dy2 <= by2) {
      const FrameBuffer& frameBuffer = gc().frame().frameBuffer();
      Rectangle::fill(frameBuffer, dx1, dy1, dx2, dy2, _bgColor);
      doDraw(frameBuffer, dx1, dy1);
    }
    // UI element is partially within the visible area
    else {
      const FrameBuffer& frameBuffer = gc().frame().frameBuffer();
      upan::uniq_ptr<uint32_t> tempBuffer(new uint32_t[width() * height()]);
      FrameBufferInfo tempFBInfo;
      tempFBInfo._width = width();
      tempFBInfo._height = height();
      tempFBInfo._pitch = width() * frameBuffer.bytesPerPixel();
      tempFBInfo._bpp = frameBuffer.bpp();
      tempFBInfo._frameBuffer = tempBuffer.get();
      FrameBuffer tempFrameBuffer(tempFBInfo);

      Rectangle::fill(tempFrameBuffer, 0, 0, width() - 1, height() - 1, _bgColor);
      doDraw(tempFrameBuffer, 0, 0);

      const int srcX1 = dx1 >= bx1 ? 0 : bx1 - dx1;
      const int srcY1 = dy1 >= by1 ? 0 : by1 - dy1;

      const int destX1 = dx1 >= bx1 ? dx1 : bx1;
      const int destY1 = dy1 >= by1 ? dy1 : by1;

      const int w = (dx2 <= bx2 ? width() : width() - (dx2 - bx2)) - srcX1;
      const int h = (dy2 <= by2 ? height() : height() - (dy2 - by2)) - srcY1;

      const int srcXOffset = srcX1 * frameBuffer.bytesPerPixel();
      const int destXOffset = destX1 * frameBuffer.bytesPerPixel();
      const int copyWidth = w * frameBuffer.bytesPerPixel();

      //printf("\n%d, %d, %d, %d, %d, %d, %d, %d, %d\n", srcX1, srcY1, destX1, destY1, w, h, srcXOffset, destXOffset, copyWidth);

      for(int y = 0; y < h; ++y) {
        int srcOffet = srcXOffset + (srcY1 + y) * width() * frameBuffer.bytesPerPixel();
        int destOffset = destXOffset + (destY1 + y) * frameBuffer.pitch();
        memcpy((void*)((uint32_t)frameBuffer.buffer() + destOffset), (void*)((uint32_t)tempFrameBuffer.buffer() + srcOffet), copyWidth);
      }
    }

    for(auto& child : children()) {
      child->draw();
    }
  }
}