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

#include <stddef.h>
#include <stdint.h>
#include <option.h>

namespace upanui {
  enum ImageComposeType {
    FIT_IN,
    STRETCH
  };

  class Image;

  class ImageResource {
  public:
    const void* data() const { return _data; }
    size_t size() const { return _size; }
    virtual Image& create() const = 0;

  protected:
    ~ImageResource() {}
    ImageResource(const void* data, size_t size) : _data(data), _size(size) {}

  private:
    const void* _data;
    const size_t _size;
  };

  class PngImageResource : public ImageResource {
  public:
    PngImageResource(const void* data, size_t size) : ImageResource(data, size) {}
    Image& create() const override;

    static const PngImageResource TEST;
    static const PngImageResource MOUSE_POINTER_CURSOR;
    static const PngImageResource MOUSE_HRESIZER_CURSOR;
    static const PngImageResource MOUSE_VRESIZER_CURSOR;
    static const PngImageResource MOUSE_UHVRESIZER_CURSOR;
    static const PngImageResource MOUSE_DHVRESIZER_CURSOR;
    static const PngImageResource CLOSE;
    static const PngImageResource UP;
    static const PngImageResource DOWN;
  };

  class BmpImageResource : public ImageResource {
  public:
    BmpImageResource(const void* data, size_t size, upan::option<uint32_t> transparentColor) : ImageResource(data, size), _transparentColor(transparentColor) {}
    BmpImageResource(const void* data, size_t size) : BmpImageResource(data, size, upan::option<uint32_t>::empty()) {}

    Image& create() const override;

    static const BmpImageResource MOUSE_CURSOR;
  private:
    upan::option<uint32_t> _transparentColor;
  };
}
