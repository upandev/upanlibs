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

#include <TextWriter.h>
#include <exception.h>
#include <GraphicsFont.h>
#include <FrameBuffer.h>
#include <BaseFrame.h>

namespace upanui {
  TextWriter::TextWriter() : _usfnContext(nullptr) {
    _xCharScale = 8;
    _yCharScale = 16;
  }

  void TextWriter::drawChar(BaseFrame& frame, byte ch, unsigned x, unsigned y, unsigned fg, unsigned bg) {
    if (_usfnContext) {
      try {
        drawUSFNChar(frame, ch, x, y, fg, bg);
        return;
      } catch(upan::exception& e) {
        _usfnContext = nullptr;
        printf("failed to render character using usfn: %s", e.ErrorMsg().c_str());
      }
    }
    x *= _xCharScale;
    y *= _yCharScale;
    if((y + _yCharScale) >= frame.frameBuffer().height() || (x + _xCharScale) >= frame.frameBuffer().width())
      return;
    fg |= 0xFF000000;
    bg |= 0xFF000000;
    const byte* font_data = GraphicsFont::Get(ch);
    bool yr = false;
    const auto pitch = frame.frameBuffer().pitch();
    const auto bytesPerPixel = frame.frameBuffer().bytesPerPixel();

    for(unsigned f = 0; f < 8; ++y) {
      unsigned lfbp = (uint32_t)frame.frameBuffer().buffer() + y * pitch + x * bytesPerPixel;
      for(unsigned i = 0x80; i != 0; i >>= 1, lfbp += bytesPerPixel)
        *(unsigned*)lfbp = font_data[f] & i ? fg : bg;

      if(yr) ++f;
      yr = !yr;
    }
    frame.touch();
  }

  void TextWriter::drawUSFNChar(BaseFrame& frame, byte ch, unsigned x, unsigned y, unsigned fg, unsigned bg) {
    //for SSFN, y is the baseline, the characters are drawn above y and hence add yScale to y --> this is only for Render() which is used for GUI
    //we don't have to do it for a standard text console display using RenderCharacter()
    //++y;
    x *= _xCharScale;
    y *= _yCharScale;

    if(y >= frame.frameBuffer().height() || (x + _xCharScale) >= frame.frameBuffer().width())
      return;

    usfn::FrameBuffer buf = {                                  /* the destination pixel buffer */
        .ptr = (uint8_t*)frame.frameBuffer().buffer(),               /* address of the buffer */
        .w = (int16_t)frame.frameBuffer().width(),                        /* width */
        .h = (int16_t)frame.frameBuffer().height(),                       /* height */
        .p = (uint16_t)frame.frameBuffer().pitch(),    /* bytes per line */
        .x = (int16_t)x,                                       /* pen position */
        .y = (int16_t)y,
        .fg = 0xFF000000 | fg,
        .bg = 0xFF000000 | bg
    };

    //    const char s[2] = { (const char)ch, '\0' };
    //    _usfnContext->Render(buf, s, true);
    _usfnContext->RenderCharacter(buf, ch);
    frame.touch();
  }

  //TODO: this is assuming 4 bytes per pixel
  //TODO: initialize y and x scale as class members and base all calculations on y/x scale instead of assuming/hardcoding
  void TextWriter::scrollDown(BaseFrame& frame) {
    const uint32_t maxSize = frame.frameBuffer().width() * frame.frameBuffer().height();
    //1 line = 16 rows as we are scaling y axis by 16
    const uint32_t oneLine = frame.frameBuffer().width() * _yCharScale;

    frame.copy((void*)((uint32_t)frame.frameBuffer().buffer() + oneLine * frame.frameBuffer().bytesPerPixel()),
               (maxSize - oneLine) * frame.frameBuffer().bytesPerPixel());

    frame.fillRect(0, frame.frameBuffer().height() - _yCharScale, frame.frameBuffer().width(), _yCharScale, 0xFF000000);
  }

  void TextWriter::drawCursor(BaseFrame& frame, uint32_t x, uint32_t y, uint32_t color) {
    x *= _xCharScale;
    y *= _yCharScale;
    frame.fillRect(x + 1, y + _yCharScale - 1, _xCharScale - 1, 1, color);
  }
}