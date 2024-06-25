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

#include <TextBuffer.h>
#include <GraphicsContext.h>

namespace upanui {
  TextBuffer::TextBuffer() : _width(0), _height(0), _bgColor(0) {
    const int maxWidth = GraphicsContext::Instance().frame().frameBuffer().width();
    const int maxHeight = GraphicsContext::Instance().frame().frameBuffer().height() + MAX_FONT_SIZE * 2;
    _drawBuffer.initLocal(maxWidth, maxHeight);
    _bgLine = new uint32_t[maxWidth];
  }

  TextBuffer::~TextBuffer() {
    delete []_bgLine;
  }

  void TextBuffer::init(int width, int height, uint32_t bgColor) {
    _width = width;
    _height = height;
    _bgColor = bgColor;
    for(int i = 0; i < _drawBuffer.width(); ++i) {
      _bgLine[i] = _bgColor;
    }
    clear();
  }

  void TextBuffer::clear(int x, int y, int width, int height) {
    _drawBuffer.fill(x, y + MAX_FONT_SIZE, width, height, _bgColor);
  }

  void TextBuffer::clear() {
    _drawBuffer.fill(0, _drawBuffer.height(), _bgLine, _drawBuffer.width() * _drawBuffer.bytesPerPixel());
  }

  void TextBuffer::move(int dy, int sy, int len) {
    dy = (dy + MAX_FONT_SIZE) * _drawBuffer.width();
    sy = (sy + MAX_FONT_SIZE) * _drawBuffer.width();
    len *= _drawBuffer.width() * _drawBuffer.bytesPerPixel();
    memmove(_drawBuffer.buffer() + dy, _drawBuffer.buffer() + sy, len);
  }

  void TextBuffer::fill(int x, int y, int width, int height, uint32_t color) {
    _drawBuffer.fill(x, y + MAX_FONT_SIZE, width, height, color);
  }

  usfn::FrameBuffer TextBuffer::initFrameBuffer(int dx, int dy, int chHeight, uint32_t fgColor, uint32_t bgColor) {
    return {
            .ptr = (uint8_t*)_drawBuffer.buffer(),
            .w = (int16_t)_drawBuffer.width(),
            .h = (int16_t)_drawBuffer.height(),
            .p = (uint16_t)_drawBuffer.pitch(),
            .x = (int16_t)dx,
            //character height is less than the line height because line height is padded with line spacing
            //therefore, it is not required do a +1 on baseDrawY unlike how it is done while calculating topY using line height
            //Remember, baseDrawY is same as the cursor Y which is the bottom Y of the character block
            .y = (int16_t)(dy + MAX_FONT_SIZE - chHeight),
            .fg = fgColor | GCoreFunctions::ALPHA_MASK,
            .bg = bgColor | GCoreFunctions::ALPHA_MASK
    };
  }

  void TextBuffer::copy(upanui::DrawBuffer& drawBuffer) {
    drawBuffer.copy(0, 0,
                    _drawBuffer.buffer() + MAX_FONT_SIZE * _drawBuffer.width(),
                    _drawBuffer.width(),
                    _width, _height, true);
  }
}