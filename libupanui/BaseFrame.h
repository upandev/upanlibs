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
    BaseFrame(const upanui::FrameBuffer& frameBuffer, const upanui::Viewport& viewport)
    : _frameBuffer(frameBuffer), _viewport(viewport), _isDirty(false) {}

    upanui::FrameBuffer& frameBuffer() {
      return _frameBuffer;
    }

    upanui::Viewport& viewport() {
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