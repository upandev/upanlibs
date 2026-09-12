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
#include <usfntypes.h>
#include <vector.h>

namespace upanui {
  class FixedCharacter {
  public:
    FixedCharacter(uint16_t ch, uint32_t fgColor, uint32_t bgColor) :
            _ch(ch), _fgColor(fgColor), _bgColor(bgColor) {}

    uint16_t getCh() const {
      return _ch;
    }

    uint32_t getFgColor() const {
      return _fgColor;
    }

    uint32_t getBgColor() const {
      return _bgColor;
    }

    void setCh(uint16_t ch) {
      _ch = ch;
    }

    void setFgColor(uint32_t fgColor) {
      _fgColor = fgColor;
    }

    void setBgColor(uint32_t bgColor) {
      _bgColor = bgColor;
    }

  private:
    uint16_t _ch;
    uint16_t _padding1;
    uint32_t _fgColor;
    uint32_t _bgColor;
  } PACKED;

  using FixedCharacters = upan::vector<FixedCharacter>;
}