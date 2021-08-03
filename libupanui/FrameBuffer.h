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

#include <cdisplay.h>

namespace upanui {
  class FrameBuffer {
  public:
    FrameBuffer(const FrameBufferInfo& frameBufferInfo) : _frameBufferInfo(frameBufferInfo) {
    }

    FrameBuffer(const FrameBuffer& frameBuffer) : _frameBufferInfo(frameBuffer._frameBufferInfo) {
    }

    uint32_t width() const {
      return _frameBufferInfo._width;
    }

    uint32_t height() const {
      return _frameBufferInfo._height;
    }

    uint32_t pitch() const {
      return _frameBufferInfo._pitch;
    }

    uint32_t bpp() const {
      return _frameBufferInfo._bpp;
    }

    uint32_t bytesPerPixel() const {
      return _frameBufferInfo._bpp / 8;
    }

    uint32_t* buffer() const {
      return _frameBufferInfo._frameBuffer;
    }

    void resetFrameBufferAddress(uint32_t* addr) {
      _frameBufferInfo._frameBuffer = addr;
    }

  private:
    FrameBufferInfo _frameBufferInfo;
  };
}