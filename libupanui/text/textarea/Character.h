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

#include "libc/include/stdlib.h"
#include "libupanui/text/usfn/usfntypes.h"
#include "libustd/ds/vector.h"

namespace upanui {
  class Character {
  public:
    static constexpr int DEFAULT_FONT_SIZE = 16;

    Character(uint16_t ch, uint8_t fontSize, uint8_t fontType,
              uint16_t style, uint32_t fgColor, uint32_t bgColor) :
            _ch(ch), _fontSize(fontSize), _fontType(fontType),
            _style(style), _fgColor(fgColor), _bgColor(bgColor) {}

    uint16_t getCh() const {
      return _ch;
    }

    uint8_t getFontSize() const {
      return _fontSize;
    }

    uint8_t getChWidth() const {
      auto w = _fontSize / 2;
      const auto e = _fontSize / DEFAULT_FONT_SIZE;

      if (_style & usfn::STYLE_BOLD) {
        w += e;
        if (_style & usfn::STYLE_ITALIC) {
          w += e * 2;
        }
      } else if (_style & usfn::STYLE_ITALIC) {
        w += e;
      }
      return w;
    }

    uint8_t getChHeight() const {
      return _fontSize;
    }

    usfn::PreloadedFonts getFontType() const {
      return (usfn::PreloadedFonts) _fontType;
    }

    uint16_t getStyle() const {
      return _style;
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

    void setFontSize(uint8_t fontSize) {
      _fontSize = fontSize;
    }

    void setFontType(usfn::PreloadedFonts fontType) {
      _fontType = fontType;
    }

    void setStyle(uint16_t style) {
      _style = style;
    }

    void setFgColor(uint32_t fgColor) {
      _fgColor = fgColor;
    }

    void setBgColor(uint32_t bgColor) {
      _bgColor = bgColor;
    }

  private:
    uint16_t _ch;
    uint8_t _padding1;
    uint8_t _fontSize;

    uint8_t _fontType;
    uint8_t _padding2;
    uint16_t _style;

    uint32_t _fgColor;
    uint32_t _bgColor;
  } PACKED;

  typedef upan::vector<Character> Characters;
}