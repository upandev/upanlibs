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

#include "TextLine.h"

namespace upanui {
  void TextLine::insert(int pos, const Character& ch) {
    _characters.insert(pos, ch);
    _width += ch.getChWidth();
    if (_characters.size() == 1) {
      _maxChHeight = ch.getChHeight();
    } else {
      _maxChHeight = upan::max(_maxChHeight, ch.getChHeight());
    }
  }

  void TextLine::remove(int from, int last) {
    if (from >= _characters.size()) {
      return;
    }

    _characters.erase(from, last);
    _width = MIN_CURSOR_WIDTH_BUFFER;
    // if the line is empty then leave the lineHeight to whatever it was before
    // if the line is not empty then recalculate the lineHeight based on remaining characters
    if (!_characters.empty()) {
      _maxChHeight = 0;
    }
    for (auto ch: _characters) {
      _width += ch.getChWidth();
      _maxChHeight = upan::max(_maxChHeight, ch.getChHeight());
    }
  }
}