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

#include <Rectangle.h>
#include <FrameBuffer.h>

namespace upanui {
  void Rectangle::fill(const FrameBuffer& framebuffer, int x1, int y1, int x2, int y2, uint32_t color) {
    uint32_t y_offset;
    for(uint32_t y = y1; y <= y2; ++y) {
      y_offset = y * framebuffer.pitch();
      for(uint32_t x = x1; x <= x2; ++x) {
        auto p = (uint32_t*)((uint32_t)framebuffer.buffer() + y_offset + x * framebuffer.bytesPerPixel());
        *p = (color | 0xFF000000);
      }
    }
  }
}