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

#include <TextArea.h>

namespace upanui {
  TextArea::TextArea(int x, int y, uint32_t width, uint32_t height) : RectangleCanvas(x, y, width, height),
    _currentFontSize(16), _currentFontType(usfn::PreloadedFonts::VGA16), _currentStyle(usfn::STYLE_REGULAR),
    _currentFGColor(0xFF000000), _currentBGColor(0xFFFFFFFF), _cursorPos(0) {
    _lines.push_back(new Line());
    _textBuffer.initLocal(MAX_FONT_SIZE * 2 + width, MAX_FONT_SIZE + height);
  }

  TextArea::~TextArea() {
    for(auto i : _lines) {
      delete i;
    }
    for(auto i : _fontContexts) {
      delete i.second;
    }
  }

  usfn::Context& TextArea::getUSFNContext(usfn::PreloadedFonts fontType) {
    auto i = _fontContexts.find(fontType);
    if (i != _fontContexts.end()) {
      return *(i->second);
    }
    auto context = new usfn::Context();
    context->Load(upanui::usfn::Context::GetPreloadedFont(fontType));
    context->Select(upanui::usfn::FAMILY_ANY, nullptr, upanui::usfn::STYLE_REGULAR, 16);
    _fontContexts.insert(FontContextMap::value_type(fontType, context));
    return *context;
  }

  void TextArea::validateCursorPos() const {
    if (_characterPos.getRow() >= _lines.size()) {
      throw upan::exception(XLOC, "invalid cursor row:%u", _characterPos.getRow());
    }

    const auto line = _lines[_characterPos.getRow()];
    if (_characterPos.getColumn() > line->size()) {
      throw upan::exception(XLOC, "invalid cursor column:%u @ line:%u", _characterPos.getColumn(), _characterPos.getRow());
    }
  }

  void TextArea::insert(uint16_t ch) {
    validateCursorPos();
    if (ch == '\n') {
      _lines.insert(_characterPos.getRow() + 1, new Line());
      _lines[_lines.size() - 1]->setMaxHeight(_currentFontSize);
      _characterPos.set(_characterPos.getRow() + 1, 0);
      _cursorPos += (width() - _cursorPos) + width() * _currentFontSize;
      scrollOnEnter();
    } else {
      auto line = _lines[_characterPos.getRow()];
      if ((line->width() + _currentFontSize / 2) >= width()) {
        //TODO: wrap the line
        return;
      }
      auto character = new Character(ch, _currentFontSize, _currentFontType, _currentStyle, _currentFGColor, _currentBGColor);
      _lines[_characterPos.getRow()]->insert(_characterPos.getColumn(), *character);
      _characterPos.set(_characterPos.getRow(), _characterPos.getColumn() + 1);
      const int x = _cursorPos % width();
      const int y = _cursorPos / width();
    }
  }

  void TextArea::scrollOnEnter() {
    auto area = width() * height();
    auto pos = _cursorPos + _currentFontSize / 2;
    if (pos < area) {
      return;
    }
    _cursorPos = (height() - 1) * width();
  }

  void TextArea::Line::insert(int pos, Character &ch) {
    _characters.insert(pos, &ch);
    _width += ch.getChWidth();
    _maxHeight = upan::max(_maxHeight, ch.getChHeight());
  }
}