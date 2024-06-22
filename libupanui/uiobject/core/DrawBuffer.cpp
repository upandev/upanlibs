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
#include <DrawBuffer.h>
#include <FrameBuffer.h>
#include <string.h>
#include <mosstd.h>
#include <GCoreFunctions.h>
#include <exception.h>
#include "algorithm.h"

namespace upanui {
  DrawBuffer::DrawBuffer() : _type(BufferType::Null), _buffer(nullptr), _width(0), _height(0), _vWidth(0), _vHeight(0), _reuseLocalBuffer(false) {
  }

  DrawBuffer::~DrawBuffer() {
    clear();
  }

  void DrawBuffer::clear() {
    if (_type == BufferType::Local) {
      delete _buffer;
    }
    _buffer = nullptr;
    _width = _vWidth = 0;
    _height = _vHeight = 0;
    _type = BufferType::Null;
  }

  void DrawBuffer::initFrom(const DrawBuffer& parent, int xOffset, int yOffset, int visualWidth, int visualHeight) {
    clear();
    if (parent.isNull()) {
      return;
    }
    _buffer = parent.buffer() + xOffset + yOffset * parent.width();
    _width = parent.width();
    _height = parent.height();
    _vWidth = visualWidth;
    _vHeight = visualHeight;
    _type = BufferType::Derived;
  }

  void DrawBuffer::initFrom(uint32_t* buffer, int width, int height) {
    clear();
    _buffer = buffer;
    _width = _vWidth = width;
    _height = _vHeight = height;
    _type = BufferType::Derived;
  }

  void DrawBuffer::cleanBuffer() {
    if (_type == BufferType::Local) {
      const auto bufSize = _width * _height;
      memset(_buffer, 0, bufSize * sizeof(uint32_t));
    }
  }

  bool DrawBuffer::initLocal(int width, int height) {
    if (_type == BufferType::Local && _width == width && _height == height) {
      return false;
    }

    if (_type == BufferType::Local && _reuseLocalBuffer && _width >= width && _height >= height) {
      _vWidth = width;
      _vHeight = height;
      cleanBuffer();
    } else {
      const auto bufSize = width * height;
      clear();
      _width = _vWidth = width;
      _height = _vHeight = height;
      _buffer = new uint32_t[bufSize];
      _type = BufferType::Local;
      cleanBuffer();
    }
    return true;
  }

  void DrawBuffer::initLocal(const FrameBuffer& parent) {
    clear();
    _width = _vWidth = parent.width();
    _height = _vHeight = parent.height();
    _buffer = parent.buffer();
    _type = BufferType::ForceLocal;
  }

  uint32_t& DrawBuffer::at(const int x, const int y) const {
    return _buffer[ x + y * width() ];
  }

  uint32_t& DrawBuffer::at(const int offset) const {
    return _buffer[ offset ];
  }

  void DrawBuffer::copy(const void *src, int len) {
    memcpy(buffer(), src, len);
  }

  void DrawBuffer::copy(int sx, int sy, const uint32_t* src, int srcWidth, int copyWidth, int copyHeight, bool directSet) {
    GCoreFunctions::PixelCache pixelCache;
    const int ex = upan::min(_vWidth, sx + copyWidth);
    const int ey = upan::min(_vHeight, sy + copyHeight);
    for(auto y = sy; y < ey; ++y) {
      auto yDestOffset = y * _width;
      auto ySrcOffset = y * srcWidth;
      if (directSet && sx < ex) {
        memcpy(&_buffer[sx + yDestOffset], &src[ySrcOffset], (ex - sx) * bytesPerPixel());
      } else {
        for (auto x = sx; x < ex; ++x) {
          GCoreFunctions::setPixel(_buffer[x + yDestOffset], src[(x - sx) + ySrcOffset], pixelCache, directSet);
        }
      }
    }
  }

  void DrawBuffer::copy(const uint32_t* src, int width, int height, bool directSet) {
    copy(0, 0, src, width, width, height, directSet);
  }

  void DrawBuffer::copy(DrawBuffer& src) {
    const int ex = upan::min(_vWidth, src.width());
    const int ey = upan::min(_vHeight, src.height());
    for(auto y = 0; y < ey; ++y) {
      auto yDestOffset = y * _width;
      auto ySrcOffset = y * src.width();
      memcpy(&_buffer[yDestOffset], &src.at(ySrcOffset), ex * bytesPerPixel());
    }
  }

  void DrawBuffer::fill(int sx, int sy, int fillWidth, int fillHeight, uint32_t color) {
    const int ex = upan::min(_vWidth, sx + fillWidth);
    const int ey = upan::min(_vHeight, sy + fillHeight);
    for(auto y = sy; y < ey; ++y) {
      auto yOffset = y * _width;
      for(auto x = sx; x < ex; ++x) {
        _buffer[x + yOffset] = color;
      }
    }
  }

  void DrawBuffer::fill(uint32_t color) {
    fill(0, 0, _vWidth, _vHeight, color);
  }
}