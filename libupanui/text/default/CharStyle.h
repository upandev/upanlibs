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

#pragma once

#include <ColorPalettes.h>

namespace upanui {
  class CharStyle {
  public:
    typedef enum {
      NO_BLINK = 0x00,
      BLINK = 0x80,
      } Blink;

    static const CharStyle& WHITE_ON_BLACK();

    CharStyle();
    CharStyle(const ColorPalettes::CP16::FGColor fgColor, const ColorPalettes::CP16::BGColor bgColor);
    CharStyle(const Blink blink, const ColorPalettes::CP16::FGColor fgColor, const ColorPalettes::CP16::BGColor bgColor);
    CharStyle(const byte style);

    void setBlink(const Blink blink);
    void setFGColor(const ColorPalettes::CP16::FGColor fgColor);
    void setBGColor(const ColorPalettes::CP16::BGColor bgColor);

    inline Blink getBlink() {
      return static_cast<Blink>(_style & BLINK);
    }
    inline ColorPalettes::CP16::FGColor getFGColor() const {
      return static_cast<ColorPalettes::CP16::FGColor>(_style & ColorPalettes::CP16::FG_WHITE);
    }
    inline ColorPalettes::CP16::BGColor getBGColor() const {
      return static_cast<ColorPalettes::CP16::BGColor>(_style & ColorPalettes::CP16::BG_WHITE);
    }

    inline byte get() { return _style; }
    inline byte get() const { return _style; }

    operator byte() const { return get(); }
    operator byte() { return get(); }

  private:
    void init(const Blink blink, const ColorPalettes::CP16::FGColor fgColor, const ColorPalettes::CP16::BGColor bgColor);

  private:
    byte _style;
  };
}