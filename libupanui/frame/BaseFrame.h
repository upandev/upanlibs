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

#pragma once

#include <FrameBuffer.h>
#include <Viewport.h>

namespace upanui {
  class BaseFrame {
  public:
    BaseFrame(const upanui::FrameBuffer& frameBuffer, const upanui::Viewport& viewport);
    virtual ~BaseFrame() {}

    void copy(const void* src, int len);
    void fillRect(int sx, int sy, int width, int height, uint32_t color);

    upanui::FrameBuffer& frameBuffer() {
      return _frameBuffer;
    }

    const upanui::Viewport& viewport() const {
      return _viewport;
    }

    virtual void touch() = 0;

  protected:
    bool _updateViewport(int x, int y, int width, int height);

  private:
    upanui::FrameBuffer _frameBuffer;
    upanui::Viewport _viewport;
  };
}