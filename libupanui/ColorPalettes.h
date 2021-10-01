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

namespace ColorPalettes {
  namespace CP16 {
    enum FGColor {
      FG_BLACK,
      FG_MAROON,
      FG_GREEN,
      FG_OLIVE,
      FG_NAVY,
      FG_PURPLE,
      FG_TEAL,
      FG_SILVER,
      FG_GRAY,
      FG_RED,
      FG_LIME,
      FG_YELLOW,
      FG_BLUE,
      FG_PINK,
      FG_AQUA,
      FG_WHITE
    };

    enum BGColor {
      BG_BLACK = (FG_BLACK << 4) & 0xF0,
      BG_MAROON = (FG_MAROON << 4) & 0xF0,
      BG_GREEN = (FG_GREEN << 4) & 0xF0,
      BG_OLIVE = (FG_OLIVE << 4) & 0xF0,
      BG_NAVY = (FG_NAVY << 4) & 0xF0,
      BG_PURPLE = (FG_PURPLE << 4) & 0xF0,
      BG_TEAL = (FG_TEAL << 4) & 0xF0,
      BG_SILVER = (FG_SILVER << 4) & 0xF0,
      BG_GRAY = (FG_GRAY << 4) & 0xF0,
      BG_RED = (FG_RED << 4) & 0xF0,
      BG_LIME = (FG_LIME << 4) & 0xF0,
      BG_YELLOW = (FG_YELLOW << 4) & 0xF0,
      BG_BLUE = (FG_BLUE << 4) & 0xF0,
      BG_PINK = (FG_PINK << 4) & 0xF0,
      BG_AQUA = (FG_AQUA << 4) & 0xF0,
      BG_WHITE = (FG_WHITE << 4) & 0xF0
    };

    uint32_t Get(int index);
    const uint32_t* GetColorTable();
  }

  namespace CP256 {
    uint32_t Get(int index);
    const uint32_t* GetColorTable();
  }
};
