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

#include <stdlib.h>
#include <usfncontext.h>

namespace upanui {
  class FrameBuffer;

  class TextWriter {
  public:
    TextWriter();
    void setFontContext(usfn::Context* c) {
      _usfnContext = c;
    }
    void drawChar(const FrameBuffer& frameBuffer, byte ch, unsigned x, unsigned y, unsigned fg, unsigned bg);
    void scrollDown(const FrameBuffer& frameBuffer);
    void drawCursor(const FrameBuffer& frameBuffer, uint32_t x, uint32_t y, uint32_t color);

  private:
    void drawUSFNChar(const FrameBuffer& frameBuffer, byte ch, unsigned x, unsigned y, unsigned fg, unsigned bg);
    void FillRect(const FrameBuffer& frameBuffer, uint32_t sx, uint32_t sy, uint32_t width, uint32_t height, uint32_t color);

    usfn::Context* _usfnContext;
    uint32_t _xCharScale;
    uint32_t _yCharScale;
  };
}