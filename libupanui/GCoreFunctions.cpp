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

#include <GCoreFunctions.h>
#include <uniq_ptr.h>
#include "algorithm.h"

namespace upanui {
  static int dcompare(double a, double b) {
    const int32_t ai = a * 100;
    const int32_t  bi = b * 100;
    return ai < bi ? -1 : ai > bi ? 1 : 0;
  }

  static uint32_t dround(double v) {
    const uint32_t a = v * 100;
    auto m = a % 100;
    if (m < 50) {
      return v;
    } else {
      return (uint32_t)v + 1;
    }
  }

  void GCoreFunctions::setPixel(uint32_t& pixel, uint32_t color, bool isDirectSet) {
    if (isDirectSet) {
      pixel = color;
    } else {
      const uint32_t ia = (color >> 24) & 0xFF;
      if (ia == 100) {
        pixel = color;
      } else if (ia > 0) {
        const uint32_t ir = (color >> 16) & 0xFF;
        const uint32_t ig = (color >> 8) & 0xFF;
        const uint32_t ib = color & 0xFF;

        const uint32_t cb = pixel & 0xFF;
        const uint32_t cg = (pixel >> 8) & 0xFF;
        const uint32_t cr = (pixel >> 16) & 0xFF;
        const uint32_t ca = (pixel >> 24) & 0xFF;

        const uint32_t caf = (100 - ia);

        pixel = (uint8_t) upan::min((100 * ia + ca * caf) / 100, 100u) << 24
                | (uint8_t) upan::min((ir * ia + cr * caf) / 100, 0xFFu) << 16
                | (uint8_t) upan::min((ig * ia + cg * caf) / 100, 0xFFu) << 8
                | (uint8_t) upan::min((ib * ia + cb * caf) / 100, 0xFFu);
      }
    }
  }

  uint32_t* GCoreFunctions::resize(const uint32_t* srcBuffer, const uint32_t srcWidth, const uint32_t srcHeight, const uint32_t destWidth, const uint32_t destHeight) {
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

        uint32_t sy = y * fy;
        double scy = y * fy;

        for(double sfy = fy; dcompare(sfy, 0.0) != 0 && sy < srcHeight;) {
          auto dy = sy + 1 - scy;
          const auto pycmp = dcompare(dy, sfy);
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

          uint32_t sx = x * fx;
          double scx = x * fx;
          for (double sfx = fx; dcompare(sfx, 0.0) != 0 && sx < srcWidth;) {
            auto dx = sx + 1 - scx;
            const auto pxcmp = dcompare(dx, sfx);
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

            double ipf = dx * dy * fa;
            uint32_t srcRGB = srcBuffer[sx + sy * srcWidth];

            da += ((srcRGB >> 24) & 0xFF) * ipf;
            dr += ((srcRGB >> 16) & 0xFF) * ipf;
            dg += ((srcRGB >> 8) & 0xFF) * ipf;
            db += (srcRGB & 0xFF) * ipf;
          }
        }
        destBuffer[x + y * destWidth] = dround(dr) << 16 | dround(dg) << 8 | dround(db) | dround(da) << 24; //0xFF000000;
      }
    }
    return destBufferU.release();
  }
}