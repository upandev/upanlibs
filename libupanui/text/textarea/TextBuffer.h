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

#include <RectangleCanvas.h>
#include <MouseEventHandler.h>
#include <map.h>
#include <vector.h>
#include <mutex.h>
#include <timer_thread.h>
#include <uniq_ptr.h>
#include <UIPosition.h>
#include <Character.h>
#include <TextLine.h>
#include <usfncontexts.h>

namespace upanui {
  class TextBuffer {
  public:
    static const uint8_t MAX_FONT_SIZE = 128;

    TextBuffer();
    ~TextBuffer();

    void init(int width, int height, uint32_t bgColor);
    void clear(int x, int  y, int width, int height);
    void clear();
    void move(int dy, int sy, int len);
    void fill(int x, int y, int width, int height, uint32_t color);
    usfn::FrameBuffer initFrameBuffer(int dx, int dy, int chHeight, uint32_t fgColor, uint32_t bgColor);
    void copy(DrawBuffer& drawBuffer);

    int width() const { return _width; }
    int height() const { return _height; }

  private:
    int _width;
    int _height;
    uint32_t _bgColor;
    DrawBuffer _drawBuffer;
    uint32_t* _bgLine;
  };
}