/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2015 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
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

namespace gccucore {
  class huffman {
  public:
    static constexpr int ZFAST_BITS = 9;
    static constexpr int ZFAST_MASK = ((1 << ZFAST_BITS) - 1);

    void build(unsigned char *sizelist, int num);

    uint16_t getFastAt(int i) {
      return fast[i];
    }

    int getMaxCodeAt(int i) {
      return maxcode[i];
    }

    uint16_t getFirstCodeAt(int i) {
      return firstcode[i];
    }

    uint16_t getFirstSymbolAt(int i) {
      return firstsymbol[i];
    }

    uint16_t getValueAt(int i) {
      return value[i];
    }

  private:
    uint16_t fast[1 << ZFAST_BITS];
    uint16_t firstcode[16];
    int maxcode[17];
    uint16_t firstsymbol[16];
    unsigned char size[288];
    uint16_t value[288];
  };
}
