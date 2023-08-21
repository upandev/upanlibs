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

#include <Image.h>
#include <GCoreFunctions.h>

namespace upanui {
  Image::Image(const uint32_t width, const uint32_t height, uint32_t* imageData) : _width(width), _height(height), _imageData(imageData) {
    calculateHasAlpha();
  }

  Image::Image(const Image& image) : _hasAlpha(image.hasAlpha()), _width(image.width()), _height(image.height()), _imageData(nullptr) {
    auto size = _width * _height;
    _imageData.reset(new uint32_t[size]);
    memcpy(_imageData.get(), image.data(), size * sizeof(uint32_t));
  }

  Image::Image(const Image& image, const uint32_t width, const uint32_t height) : _width(image.width()), _height(image.height()), _imageData(nullptr) {
    auto size = _width * _height;
    _imageData.reset(new uint32_t[size]);
    memcpy(_imageData.get(), image.data(), size * sizeof(uint32_t));
    calculateHasAlpha();
    resize(width, height);
  }

  void Image::resize(const uint32_t width, const uint32_t height) {
    if (width == _width && height == _height) {
      return;
    }
    const auto destWidth = width;
    const auto destHeight = height;
    const auto srcWidth = _width;
    const auto srcHeight = _height;

    auto destSize = destWidth * destHeight;
    upan::uniq_ptr<uint32_t> destBufferU(new uint32_t[destSize]);
    auto destBuffer = destBufferU.get();

    const double fx = srcWidth * 1.0 / destWidth;
    const double fy = srcHeight * 1.0 / destHeight;

    const double fa = 1.0 / (fx * fy);

    for(uint32_t y = 0; y < destHeight; ++y) {
      for(uint32_t x = 0; x < destWidth; ++x) {
        double dr = 0;
        double dg = 0;
        double db = 0;
        double da = 0;

        double scy = y * fy;
        auto sy = (uint32_t)scy;

        for(double sfy = fy; GCoreFunctions::dcompare(sfy, 0.0) != 0 && sy < srcHeight;) {
          auto dy = sy + 1 - scy;
          const auto pycmp = GCoreFunctions::dcompare(dy, sfy);
          if (pycmp < 0) {
            ++sy;
            scy = sy;
            sfy -= dy;
          } else if(pycmp == 0) {
            ++sy;
            scy = sy;
            sfy = 0;
          } else {
            dy = sfy;
            scy += dy;
            sfy = 0;
          }

          if (sy >= srcHeight) {
            break;
          }

          double scx = x * fx;
          auto sx = (uint32_t)scx;
          for (double sfx = fx; GCoreFunctions::dcompare(sfx, 0.0) != 0 && sx < srcWidth;) {
            auto dx = sx + 1 - scx;
            const auto pxcmp = GCoreFunctions::dcompare(dx, sfx);
            if (pxcmp < 0) {
              ++sx;
              scx = sx;
              sfx -= dx;
            } else if(pxcmp == 0) {
              ++sx;
              scx = sx;
              sfx = 0;
            } else {
              dx = sfx;
              scx += dx;
              sfx = 0;
            }

            if (sx >= srcWidth) {
              break;
            }

            double ipf = dx * dy * fa;
            uint32_t srcRGB = _imageData.get()[sx + sy * srcWidth];

            da += ((srcRGB >> 24) & 0xFF) * ipf;
            dr += ((srcRGB >> 16) & 0xFF) * ipf;
            dg += ((srcRGB >> 8) & 0xFF) * ipf;
            db += (srcRGB & 0xFF) * ipf;
          }
        }
        if (!_hasAlpha) {
          da = GCoreFunctions::MAX_ALPHA;
        }
        destBuffer[x + y * destWidth] = roundtoi(dr) << 16 | roundtoi(dg) << 8 | roundtoi(db) | roundtoi(da) << 24; //0xFF000000;
      }
    }
    _width = destWidth;
    _height = destHeight;
    _imageData.reset(destBufferU.release());
    calculateHasAlpha();
  }

  void Image::calculateHasAlpha() {
    const auto size = _width * _height;
    auto data = _imageData.get();
    uint32_t i;
    for(i = 0; i < size; ++i) {
      const uint32_t p = data[i];
      if ((p & GCoreFunctions::ALPHA_MASK) != GCoreFunctions::ALPHA_MASK) {
        break;
      }
    }
    _hasAlpha = i < size;
  }
}
