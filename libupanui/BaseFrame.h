/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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

#pragma once

#include <FrameBuffer.h>
#include <Viewport.h>
#include <atomicop.h>

namespace upanui {
  class BaseFrame {
  public:
    BaseFrame(const upanui::FrameBuffer& frameBuffer, const upanui::Viewport& viewport);

    void resetFrameBufferAddress(uint32_t* frameAddr);
    void copy(const void* src, int len);
    void fillRect(uint32_t sx, uint32_t sy, uint32_t width, uint32_t height, uint32_t color);

    const upanui::FrameBuffer& frameBuffer() const {
      return _frameBuffer;
    }

    const upanui::Viewport& viewport() const {
      return _viewport;
    }

    bool isDirty() {
      return _isDirty.get();
    }

    void touch() {
      _isDirty.set(true);
    }

    void clean() {
      _isDirty.set(false);
    }

  private:
    upanui::FrameBuffer _frameBuffer;
    upanui::Viewport _viewport;
    upan::atomic::integral<bool> _isDirty;
  };
}