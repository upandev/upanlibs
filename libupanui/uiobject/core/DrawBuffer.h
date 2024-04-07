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

#include <stdlib.h>

namespace upanui {
  class FrameBuffer;

  class DrawBuffer {
  public:
    DrawBuffer();
    ~DrawBuffer();

    void clear();
    void initFrom(const DrawBuffer& parent, int xOffset, int yOffset, int visualWidth, int visualHeight);
    void initFrom(uint32_t* buffer, int width, int height);
    bool initLocal(int width, int height);
    void initLocal(const FrameBuffer& frameBuffer);

    int width() const { return _width; }
    int height() const { return _height; }
    int bytesPerPixel() const { return sizeof(uint32_t); }
    int pitch() const { return width() * bytesPerPixel(); }
    bool isLocal() const { return _type == BufferType::Local || _type == BufferType::ForceLocal; }
    bool isNull() const { return _type == BufferType::Null; }
    uint32_t* buffer() const { return _buffer; }
    uint32_t& at(int x, int y) const;
    uint32_t& at(int offset)  const;
    void cleanBuffer();

    void copy(const void* src, int len);
    void copy(int sx, int sy, const uint32_t* src, int srcWidth, int copyWidth, int copyHeight, bool directSet);
    void copy(const uint32_t* src, int width, int height, bool directSet);
    void copy(DrawBuffer& src);
    void fill(int sx, int sy, int fillWidth, int fillHeight, uint32_t color);
    void fill(uint32_t color);

  private:
    enum BufferType { Null, Local, ForceLocal, Derived };
    BufferType _type;
    uint32_t* _buffer;
    int _width;
    int _height;
    int _vWidth;
    int _vHeight;
  };
}