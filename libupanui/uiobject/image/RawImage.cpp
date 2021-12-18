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

#include <RawImage.h>
#include <ImageAlgo.h>

namespace upanui {
  RawImage::RawImage(const Image& image) : Image(image), _imageBuffer(nullptr) {
    auto size = width() * height();
    _imageBuffer.reset(new uint32_t[size]);
    memcpy(_imageBuffer.get(), image.data(), size);
  }

  RawImage::RawImage(const Image& image, uint32_t newWidth, uint32_t newHeight)
    : Image(image.x(), image.y(), newWidth, newHeight), _imageBuffer(nullptr) {
    _imageBuffer.reset(ImageAlgo::resize(image.data(), image.width(), image.height(), newWidth, newHeight));
  }

  RawImage::~RawImage() noexcept {
  }

  void RawImage::draw() {
  }

  void RawImage::drawTopDown() {
  }

  void RawImage::drawToTop() {
  }
}
