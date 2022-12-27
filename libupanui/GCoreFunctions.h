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

#include <stdint.h>

namespace upanui {
  class GCoreFunctions {
  public:
    static constexpr uint32_t ALPHA_MASK = (100 << 24);

    class PixelCache {
    public:
      PixelCache() : _cDest(0), _cSrc(0), _cRes(0) {}
      uint32_t _cDest;
      uint32_t _cSrc;
      uint32_t _cRes;
    };

    static void setPixel(uint32_t& pixel, uint32_t color, PixelCache& pixelCache, bool isDirectSet);
    static uint32_t* resize(const uint32_t* srcBuffer, const uint32_t srcWidth, const uint32_t srcHeight, const uint32_t destWidth, const uint32_t destHeight);
  };
}