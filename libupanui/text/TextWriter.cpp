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

namespace upanui {
  TextWriter::TextWriter() : _usfnContext(nullptr) {
    _xCharScale = 8;
    _yCharScale = 16;
  }

  void TextWriter::drawChar(const FrameBuffer& frameBuffer, byte ch, unsigned x, unsigned y, unsigned fg, unsigned bg) {
    if (_usfnContext) {
      try {
        drawUSFNChar(frameBuffer, ch, x, y, fg, bg);
        return;
      } catch(upan::exception& e) {
        _usfnContext = nullptr;
        printf("failed to render character using usfn: %s", e.ErrorMsg().c_str());
      }
    }
    x *= _xCharScale;
    y *= _yCharScale;
    if((y + _yCharScale) >= frameBuffer.height() || (x + _xCharScale) >= frameBuffer.width())
      return;
    fg |= 0xFF000000;
    bg |= 0xFF000000;
    const byte* font_data = GraphicsFont::Get(ch);
    bool yr = false;
    const auto pitch = frameBuffer.pitch();
    const auto bytesPerPixel = frameBuffer.bytesPerPixel();

    for(unsigned f = 0; f < 8; ++y) {
      unsigned lfbp = (uint32_t)frameBuffer.buffer() + y * pitch + x * bytesPerPixel;
      for(unsigned i = 0x80; i != 0; i >>= 1, lfbp += bytesPerPixel)
        *(unsigned*)lfbp = font_data[f] & i ? fg : bg;

      if(yr) ++f;
      yr = !yr;
    }
  }

  void TextWriter::drawUSFNChar(const FrameBuffer& frameBuffer, byte ch, unsigned x, unsigned y, unsigned fg, unsigned bg) {
    //for SSFN, y is the baseline, the characters are drawn above y and hence add yScale to y --> this is only for Render() which is used for GUI
    //we don't have to do it for a standard text console display using RenderCharacter()
    //++y;
    x *= _xCharScale;
    y *= _yCharScale;

    if(y >= frameBuffer.height() || (x + _xCharScale) >= frameBuffer.width())
      return;

    usfn::FrameBuffer buf = {                                  /* the destination pixel buffer */
        .ptr = (uint8_t*)frameBuffer.buffer(),               /* address of the buffer */
        .w = (int16_t)frameBuffer.width(),                        /* width */
        .h = (int16_t)frameBuffer.height(),                       /* height */
        .p = (uint16_t)frameBuffer.pitch(),    /* bytes per line */
        .x = (int16_t)x,                                       /* pen position */
        .y = (int16_t)y,
        .fg = 0xFF000000 | fg,
        .bg = 0xFF000000 | bg
    };

    //    const char s[2] = { (const char)ch, '\0' };
    //    _usfnContext->Render(buf, s, true);
    _usfnContext->RenderCharacter(buf, ch);
  }

  //TODO: this is assuming 4 bytes per pixel
  //TODO: initialize y and x scale as class members and base all calculations on y/x scale instead of assuming/hardcoding
  void TextWriter::scrollDown(const FrameBuffer& frameBuffer) {
    const uint32_t maxSize = frameBuffer.width() * frameBuffer.height();
    //1 line = 16 rows as we are scaling y axis by 16
    const uint32_t oneLine = frameBuffer.width() * 16;

    memcpy((void*)frameBuffer.buffer(),
           (void*)((uint32_t)frameBuffer.buffer() + oneLine * frameBuffer.bytesPerPixel()),
           (maxSize - oneLine) * frameBuffer.bytesPerPixel());
    unsigned i = maxSize - oneLine;
    unsigned* lfb = (unsigned*)(frameBuffer.buffer());
    for(; i < maxSize; ++i)
      lfb[i] = 0xFF000000;
  }

  void TextWriter::drawCursor(const FrameBuffer& frameBuffer, uint32_t x, uint32_t y, uint32_t color) {
    x *= _xCharScale;
    y *= _yCharScale;
    if(y >= frameBuffer.height() || (x + _xCharScale) >= frameBuffer.width())
      return;
    FillRect(frameBuffer, x + 1, y + _yCharScale - 1, _xCharScale - 1, 1, color);
   }

   void TextWriter::FillRect(const FrameBuffer& frameBuffer, uint32_t sx, uint32_t sy, uint32_t width, uint32_t height, uint32_t color) {
    uint32_t y_offset;
    for(uint32_t y = sy; y < (sy + height) && y < frameBuffer.height(); ++y) {
      y_offset = y * frameBuffer.pitch();
      for(uint32_t x = sx; x < (sx + width) && x < frameBuffer.width(); ++x) {
        auto p = (uint32_t*)((uint32_t)frameBuffer.buffer() + y_offset + x * frameBuffer.bytesPerPixel());
        *p = (color | 0xFF000000);
      }
    }
  }
}