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

#include <ImageCanvas.h>
#include <Image.h>

namespace upanui {
  ImageCanvas::ImageCanvas(const Image& image, ImageComposeType composeType,
                           const int x, const int y, const uint32_t width, const uint32_t height)
    : RectangleCanvas(x, y, width, height), _image(new Image(image)), _composeType(composeType) {
  }

  ImageCanvas::ImageCanvas(Image* inImage, ImageComposeType composeType,
                           const int x, const int y, const uint32_t width, const uint32_t height)
      : RectangleCanvas(x, y, width, height), _image(inImage), _composeType(composeType) {
  }

  void ImageCanvas::setImage(const Image& image) {
    _image.reset(new Image(image, width(), height()));
    notifyChange(ChangeState::Content);
  }

  bool ImageCanvas::hasAlphaLocal() {
    return _image->hasAlpha();
  }

  void ImageCanvas::doDraw() {
    drawBuffer().cleanBuffer();

    if (_image.toOption().isEmpty()) {
      return;
    }

    uint32_t nw, nh;
    switch(_composeType) {
      case STRETCH:
        nw = upan::max(width(), _image->width());
        nh = upan::max(height(), _image->height());
        break;

      case FIT_IN:
      default:
        nw = upan::min(width(), _image->width());
        nh = upan::min(height(), _image->height());
        break;
    }

    _image->resize(nw, nh);

    drawBuffer().copy(_image->data(), _image->width(), _image->height(), !hasAlphaLocal());
  }
}