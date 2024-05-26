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
#include <Character.h>

namespace upanui {
  class TextArea;

  class TextLine {
  public:
    static const int MIN_CURSOR_WIDTH_BUFFER = 8;
    static const int DEFAULT_LINE_SPACE = 4;

    TextLine(uint8_t defaultHeight, TextArea& textArea) : _width(MIN_CURSOR_WIDTH_BUFFER), _maxChHeight(defaultHeight), _wrapped(false), _textArea(textArea) {}
    TextLine(const TextLine&) = delete;
    TextLine& operator=(const TextLine&) = delete;

    void insert(int pos, const Character& ch);
    void remove(int from, int last);

    int width() const { return _width; }
    uint8_t lineHeight() const { return _maxChHeight + DEFAULT_LINE_SPACE; }
    bool wrapped() const { return _wrapped; }
    void wrapped(bool wrapped) { _wrapped = wrapped; }
    int size() const { return _characters.size(); }

    const Characters& characters() const { return _characters; }
    const Character& characters(int i) const { return _characters[i]; }

    void render(int charX, int charY, int baseDrawY);

  private:
    Characters _characters;
    int _width;
    uint8_t _maxChHeight;
    bool _wrapped;
    TextArea& _textArea;
  };
}