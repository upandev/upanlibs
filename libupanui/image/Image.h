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

#include <UIElement.h>
#include <uniq_ptr.h>

namespace upanui {
  class Image {
  public:
    Image(const uint32_t width, const uint32_t height, uint32_t* imageData);
    Image(const Image&);
    Image(const Image& image, const uint32_t width, const uint32_t height);
    ~Image() {}

    uint32_t width() const {
      return _width;
    }

    uint32_t height() const {
      return _height;
    }

    const uint32_t* data() const {
      return const_cast<Image*>(this)->_imageData.get();
    }

    void resize(const uint32_t width, const uint32_t height);

  private:
    uint32_t _width;
    uint32_t _height;
    //assuming 4 bytes per pixel
    upan::uniq_ptr<uint32_t> _imageData;
  };
}
