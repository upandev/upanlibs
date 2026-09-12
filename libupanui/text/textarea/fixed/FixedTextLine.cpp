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

#include <FixedTextLine.h>
#include <FixedTextArea.h>

namespace upanui {
  void FixedTextLine::insert(int pos, const FixedCharacter& ch) {
    _fixedCharacters.insert(pos, ch);
  }

  void FixedTextLine::remove(int from, int last) {
    if (from == last || from >= _fixedCharacters.size()) {
      return;
    }
    _fixedCharacters.erase(from, last);
  }

  void FixedTextLine::render(int charX, int charY, int baseDrawY) {
    int topY = baseDrawY - _textArea.lineHeight() + 1;
    if (topY >= _textArea.height() || baseDrawY < 0) {
      return;
    }

    auto drawX = _textArea.leftMargin() + charX * _textArea.charWidth();
    _textArea.textBuffer().clear(drawX, topY, _textArea.width() - drawX, _textArea.lineHeight());

    for(int i = charX; i < _fixedCharacters.size(); ++i) {
      const auto& ch = _fixedCharacters[i];
      usfn::FrameBuffer buf = _textArea.textBuffer().initFrameBuffer(drawX,
                                                                     baseDrawY,
                                                                     _textArea.charHeight(),
                                                                     ch.getFgColor(),
                                                                     ch.getBgColor());
      _textArea.textBuffer().fill(drawX, topY, _textArea.charWidth(), _textArea.lineHeight(), _textArea.getChBgColor(i, charY, ch));

      char str[2] = { (char)ch.getCh(), '\0'};
      auto& sfnContext = _textArea.fontContexts().get(_textArea.fontType(), _textArea.fontSize(), _textArea.fontStyle());
      sfnContext.RenderText(buf, str, true, false);

      drawX += _textArea.charWidth();
    }
  }

  upan::string FixedTextLine::toString(int pos) const {
    upan::string line;
    for(int i = pos; i < _fixedCharacters.size(); ++i) {
      line += (char)_fixedCharacters[i].getCh();
    }
    return line;
  }
}