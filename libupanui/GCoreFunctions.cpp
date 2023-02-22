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
  int GCoreFunctions::dcompare(double a, double b) {
    const int32_t ai = a * 100;
    const int32_t  bi = b * 100;
    return ai < bi ? -1 : ai > bi ? 1 : 0;
  }

  uint32_t GCoreFunctions::percentToAlpha(uint32_t percent) {
    return MAX_ALPHA * upan::min(percent, 100u) / 100;
  }

  void GCoreFunctions::setPixel(uint32_t& pixel, uint32_t color, PixelCache& pixelCache, bool isDirectSet) {
    if (isDirectSet) {
      pixel = color;
    } else {
      const uint32_t ia = (color >> 24) & 0xFF;
      const uint32_t oa = (pixel >> 24) & 0xFF;
      if (ia > 0) {
        if (ia == MAX_ALPHA || oa == 0) {
          pixel = color;
        } else {
          if (pixelCache._cDest == pixel && pixelCache._cSrc == color) {
            pixel = pixelCache._cRes;
          } else {
            pixelCache._cDest = pixel;
            pixelCache._cSrc = color;
            const uint32_t ir = (color >> 16) & 0xFF;
            const uint32_t ig = (color >> 8) & 0xFF;
            const uint32_t ib = color & 0xFF;

            const uint32_t cb = pixel & 0xFF;
            const uint32_t cg = (pixel >> 8) & 0xFF;
            const uint32_t cr = (pixel >> 16) & 0xFF;
            const uint32_t ca = (pixel >> 24) & 0xFF;

            const uint32_t caf = (MAX_ALPHA - ia);

            pixel = (uint8_t) upan::min((MAX_ALPHA * ia + ca * caf) / MAX_ALPHA, 0xFFu) << 24
                    | (uint8_t) upan::min((ir * ia + cr * caf) / MAX_ALPHA, 0xFFu) << 16
                    | (uint8_t) upan::min((ig * ia + cg * caf) / MAX_ALPHA, 0xFFu) << 8
                    | (uint8_t) upan::min((ib * ia + cb * caf) / MAX_ALPHA, 0xFFu);
            pixelCache._cRes = pixel;
          }
        }
      }
    }
  }
}