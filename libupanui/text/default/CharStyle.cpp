/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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

#include <CharStyle.h>

namespace upanui {

// This constant is used by kernel even before C++ global static initialization is done. Hence, put this in a static function to make sure it is initialized when used.
const CharStyle& CharStyle::WHITE_ON_BLACK() {
  static CharStyle val(ColorPalettes::CP16::FG_WHITE, ColorPalettes::CP16::BG_BLACK);
  return val;
}

CharStyle::CharStyle() {
  init(NO_BLINK, ColorPalettes::CP16::FG_WHITE, ColorPalettes::CP16::BG_BLACK);
}

CharStyle::CharStyle(const ColorPalettes::CP16::FGColor fgColor, const ColorPalettes::CP16::BGColor bgColor) {
  init(NO_BLINK, fgColor, bgColor);
}

CharStyle::CharStyle(const Blink blink, const ColorPalettes::CP16::FGColor fgColor, const ColorPalettes::CP16::BGColor bgColor) {
  init(blink, fgColor, bgColor);
}

CharStyle::CharStyle(const byte style) {
  init(static_cast<Blink>(style & BLINK),
       static_cast<ColorPalettes::CP16::FGColor>(style & ColorPalettes::CP16::FG_WHITE),
       static_cast<ColorPalettes::CP16::BGColor>(style & ColorPalettes::CP16::BG_WHITE));
}

void CharStyle::init(const CharStyle::Blink blink, const ColorPalettes::CP16::FGColor fgColor, const ColorPalettes::CP16::BGColor bgColor) {
  _style = blink | fgColor | bgColor;
}

void CharStyle::setBlink(const CharStyle::Blink blink) {
  init(blink, getFGColor(), getBGColor());
}

void CharStyle::setFGColor(const ColorPalettes::CP16::FGColor fgColor) {
  init(getBlink(), fgColor, getBGColor());
}

void CharStyle::setBGColor(const ColorPalettes::CP16::BGColor bgColor) {
  init(getBlink(), getFGColor(), bgColor);
}
}