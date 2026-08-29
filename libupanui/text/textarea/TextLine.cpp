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

#include <TextLine.h>
#include <TextArea.h>

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
    if (from == last || from >= _characters.size()) {
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

  void TextLine::render(int charX, int charY, int baseDrawY) {
    int topY = baseDrawY - lineHeight() + 1;
    if (topY >= _textArea.height() || baseDrawY < 0) {
      return;
    }

    auto drawX = _textArea.leftMargin();
    for(int i = 0; i < charX; ++i) {
      drawX += _characters[i].getChWidth();
    }
    _textArea.textBuffer().clear(drawX, topY, _textArea.width() - drawX, lineHeight());

    for(int i = charX; i < _characters.size(); ++i) {
      const auto& ch = _characters[i];
      usfn::FrameBuffer buf = _textArea.textBuffer().initFrameBuffer(drawX,
                                                                     baseDrawY,
                                                                     ch.getChHeight(),
                                                                     _textArea.currentFgColor(),
                                                                     _textArea.backgroundColor());
      _textArea.textBuffer().fill(drawX, topY, ch.getChWidth(), lineHeight(), _textArea.getChBgColor(i, charY, ch));

      char str[2] = { (char)ch.getCh(), '\0'};
      auto& sfnContext = _textArea.fontContexts().get(ch.getFontType(), ch.getFontSize(), ch.getStyle());
      sfnContext.RenderText(buf, str, true, false);

      drawX += ch.getChWidth();
    }
  }

  upan::string TextLine::toString(int pos) const {
    upan::string line;
    for(int i = pos; i < _characters.size(); ++i) {
      line += (char)_characters[i].getCh();
    }
    return line;
  }
}