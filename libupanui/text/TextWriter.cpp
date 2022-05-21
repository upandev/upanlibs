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

#include <TextWriter.h>
#include <exception.h>
#include <GraphicsFont.h>
#include <FrameBuffer.h>
#include <BaseFrame.h>
#include <GCoreFunctions.h>
#include <UIObject.h>
#include <DrawBuffer.h>

namespace upanui {
  TextWriter::TextWriter() : _usfnContext(nullptr) {
    _xCharScale = 8;
    _yCharScale = 16;
  }

  void TextWriter::drawChar(upanui::UIObject& parent, byte ch, unsigned x, unsigned y, unsigned fg, unsigned bg) {
    if (_usfnContext) {
      try {
        drawUSFNChar(parent, ch, x, y, fg, bg);
        return;
      } catch(upan::exception& e) {
        _usfnContext = nullptr;
        printf("failed to render character using usfn: %s", e.ErrorMsg().c_str());
      }
    }
    x *= _xCharScale;
    y *= _yCharScale;
    auto& drawBuffer = parent.drawBuffer();
    if((y + _yCharScale) >= drawBuffer.height() || (x + _xCharScale) >= drawBuffer.width())
      return;
    const byte* font_data = GraphicsFont::Get(ch);
    bool yr = false;
    const auto pitch = drawBuffer.pitch();
    const auto bytesPerPixel = drawBuffer.bytesPerPixel();

    for(unsigned f = 0; f < 8; ++y) {
      unsigned lfbp = (uint32_t)drawBuffer.buffer() + y * pitch + x * bytesPerPixel;
      for(unsigned i = 0x80; i != 0; i >>= 1, lfbp += bytesPerPixel)
        *(unsigned*)lfbp = font_data[f] & i ? fg : bg;

      if(yr) ++f;
      yr = !yr;
    }
    parent.draw();
  }

  void TextWriter::drawUSFNChar(upanui::UIObject& parent, byte ch, unsigned x, unsigned y, unsigned fg, unsigned bg) {
    //for SSFN, y is the baseline, the characters are drawn above y and hence add yScale to y --> this is only for Render() which is used for GUI
    //we don't have to do it for a standard text console display using RenderCharacter()
    //++y;
    x *= _xCharScale;
    y *= _yCharScale;
    auto& drawBuffer = parent.drawBuffer();
    if(y >= drawBuffer.height() || (x + _xCharScale) >= drawBuffer.width())
      return;

    usfn::FrameBuffer buf = {                                  /* the destination pixel buffer */
        .ptr = (uint8_t*)drawBuffer.buffer(),               /* address of the buffer */
        .w = (int16_t)drawBuffer.width(),                        /* width */
        .h = (int16_t)drawBuffer.height(),                       /* height */
        .p = (uint16_t)drawBuffer.pitch(),    /* bytes per line */
        .x = (int16_t)x,                                       /* pen position */
        .y = (int16_t)y,
        .fg = fg | GCoreFunctions::ALPHA_MASK,
        .bg = bg | GCoreFunctions::ALPHA_MASK
    };

    //    const char s[2] = { (const char)ch, '\0' };
    //    _usfnContext->Render(buf, s, true);
    _usfnContext->RenderCharacter(buf, ch);
    parent.draw();
  }

  //TODO: this is assuming 4 bytes per pixel
  //TODO: initialize y and x scale as class members and base all calculations on y/x scale instead of assuming/hardcoding
  void TextWriter::scrollDown(upanui::UIObject& parent) {
    auto& drawBuffer = parent.drawBuffer();
    const uint32_t maxSize = drawBuffer.width() * drawBuffer.height();
    //1 line = 16 rows as we are scaling y axis by 16
    const uint32_t oneLine = drawBuffer.width() * _yCharScale;

    drawBuffer.copy((void*)((uint32_t)drawBuffer.buffer() + oneLine * drawBuffer.bytesPerPixel()), (maxSize - oneLine) * drawBuffer.bytesPerPixel());

    drawBuffer.fill(0, drawBuffer.height() - _yCharScale, drawBuffer.width(), _yCharScale, GCoreFunctions::ALPHA_MASK);
    parent.draw();
  }

  void TextWriter::drawCursor(upanui::UIObject& parent, uint32_t x, uint32_t y, uint32_t color) {
    x *= _xCharScale;
    y *= _yCharScale;
    parent.drawBuffer().fill(x + 1, y + _yCharScale - 1, _xCharScale - 1, 1, color | GCoreFunctions::ALPHA_MASK);
    parent.draw();
  }
}