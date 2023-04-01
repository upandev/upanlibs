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
#include <GCoreFunctions.h>
#include <KeyboardEvent.h>
#include <GraphicsContext.h>
#include <kb.h>

namespace upanui {
  TextArea::TextArea(int x, int y, uint32_t width, uint32_t height) : RectangleCanvas(x, y, width, height),
    _currentFontSize(16), _currentFontType(usfn::PreloadedFonts::VGA16), _currentStyle(usfn::STYLE_REGULAR),
    _currentFGColor(0x000000 | GCoreFunctions::ALPHA_MASK), _currentBGColor(0xFFFFFF | GCoreFunctions::ALPHA_MASK),
    _cursorBlinkThread(*this) {
    _lines.push_back(new Line(_currentFontSize));
    backgroundColor(_currentBGColor);
    _textBuffer.initLocal(width, MAX_FONT_SIZE * 2 + height);
    _textBuffer.fill(backgroundColorWithAlpha());
    _cursorPos.set(0, _currentFontSize);
    _characterPos.set(0, 0);
    _cursorBlinkThread.start();
    UIObjectImpl::captureMouseEvents(true);
  }

  TextArea::~TextArea() {
    for(auto i : _lines) {
      delete i;
    }
    for(auto i : _fontContexts) {
      delete i.second;
    }
    _cursorBlinkThread.stop();
  }

  usfn::Context& TextArea::getUSFNContext(usfn::PreloadedFonts fontType) {
    auto i = _fontContexts.find(fontType);
    if (i != _fontContexts.end()) {
      return *(i->second);
    }
    auto context = new usfn::Context();
    context->Load(upanui::usfn::Context::GetPreloadedFont(fontType));
    context->Select(upanui::usfn::FAMILY_ANY, nullptr, upanui::usfn::STYLE_REGULAR, _currentFontSize);
    _fontContexts.insert(FontContextMap::value_type(fontType, context));
    return *context;
  }

  void TextArea::validateCursorPos() const {
    if (_characterPos.y() >= _lines.size()) {
      throw upan::exception(XLOC, "invalid cursor row:%u", _characterPos.y());
    }

    const auto line = _lines[_characterPos.y()];
    if (_characterPos.x() > line->size()) {
      throw upan::exception(XLOC, "invalid cursor column:%u @ line:%u", _characterPos.x(), _characterPos.y());
    }
  }

  void TextArea::enter() {
    Line& curLine = *_lines[_characterPos.y()];
    Line& newLine = *new Line(_currentFontSize);
    _lines.insert(_characterPos.y() + 1, &newLine);

    for(int i = _characterPos.x(), j = 0; i < curLine.size(); ++i, ++j) {
      newLine.insert(j, *curLine.characters()[i]);
    }

    curLine.remove(_characterPos.x(), curLine.characters().size());
    RenderLine(curLine, _characterPos.x(), _cursorPos.y());

    _characterPos.set(0, _characterPos.y() + 1);

    updateCursor(false);

    auto cursorY = _cursorPos.y() + _currentFontSize;
    if (cursorY < height()) {
      auto srcY = MAX_FONT_SIZE + _cursorPos.y() + 1;
      auto destY = MAX_FONT_SIZE + cursorY + 1;
      memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + srcY * width(), (height() - cursorY - 1) * width() * _textBuffer.bytesPerPixel());
    } else {
      auto moveY = cursorY - (height() - 1);
      auto srcY = MAX_FONT_SIZE + moveY;
      auto destY = MAX_FONT_SIZE;
      memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + srcY * width(), (_cursorPos.y() - moveY) * width() * _textBuffer.bytesPerPixel());
      cursorY = height() - 1;
    }

    RenderLine(newLine, 0, cursorY);

    _cursorPos.set(0, cursorY);
    updateCursor(true);
    notifyChange(ChangeState::Content);

  }

  void TextArea::insert(uint16_t ch) {
    if (is_command_key(ch)) {
      return;
    }

    auto line = _lines[_characterPos.y()];
    if ((line->width() + _currentFontSize / 2) >= width()) {
      //TODO: wrap the line
      return;
    }

    if (line->maxHeight() < _currentFontSize) {
      updateCursor(_cursorPos.x(), _cursorPos.y() + _currentFontSize - line->maxHeight());
    }
    auto character = new Character(ch, _currentFontSize, _currentFontType, _currentStyle, _currentFGColor, _currentBGColor);
    line->insert(_characterPos.x(), *character);
    RenderLine(*line, _characterPos.x(), _cursorPos.y());
    _characterPos.set(_characterPos.x() + 1, _characterPos.y());
    updateCursor(_cursorPos.x() + character->getChWidth(), _cursorPos.y());
  }

  void TextArea::removech() {
    auto line = _lines[_characterPos.y()];
    if (_characterPos.x() == line->characters().size()) {
      return;
    }
    line->remove(_characterPos.x(), _characterPos.x() + 1);
    RenderLine(*line, _characterPos.x(), _cursorPos.y());
    notifyChange(ChangeState::Content);
  }

  void TextArea::backspace() {
    if (_characterPos.x() == 0) {
      return;
    }
    moveleft();
    removech();
  }

  void TextArea::RenderLine(const Line& line, int charX, int baseDrawY) {
    auto drawX = 0;
    auto& characters = line.characters();
    for(int i = 0; i < charX; ++i) {
      drawX += characters[i]->getChWidth();
    }
    clearArea(drawX, baseDrawY - line.maxHeight(), width() - drawX, line.maxHeight() + 1);

    for(int i = charX; i < characters.size(); ++i) {
      auto ch = characters[i];
      usfn::FrameBuffer buf = {
          .ptr = (uint8_t*)_textBuffer.buffer(),
          .w = (int16_t)_textBuffer.width(),
          .h = (int16_t)_textBuffer.height(),
          .p = (uint16_t)_textBuffer.pitch(),
          .x = (int16_t)drawX,
          .y = (int16_t)(baseDrawY + MAX_FONT_SIZE - ch->getChHeight()),
          .fg = _currentFGColor | GCoreFunctions::ALPHA_MASK,
          .bg = _currentBGColor | GCoreFunctions::ALPHA_MASK
      };
      getUSFNContext(getCurrentFontType()).RenderCharacter(buf, ch->getCh());
      drawX += characters[charX]->getChWidth();
    }
  }

  void TextArea::moveup() {
    if (_characterPos.y() == 0) {
      return;
    }
    auto curLine = _lines[_characterPos.y()];
    auto prevLine = _lines[_characterPos.y() - 1];

    const auto& characters = prevLine->characters();
    auto cursorY = _cursorPos.y() - curLine->maxHeight();
    int cursorX = 0;
    int charX = 0;
    for (; charX < characters.size(); ++charX) {
      auto ch = characters[charX];
      if ((cursorX + ch->getChWidth()) > _cursorPos.x()) {
        break;
      }
      cursorX += ch->getChWidth();
    }
    _characterPos.set(charX, _characterPos.y() - 1);

    if (cursorY >= prevLine->maxHeight()) {
      updateCursor(cursorX, cursorY);
    } else {
      auto srcY = MAX_FONT_SIZE + cursorY;
      auto destY = MAX_FONT_SIZE + prevLine->maxHeight();

      updateCursor(false);

      memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + srcY * width(), (height() - cursorY) * width() * _textBuffer.bytesPerPixel());
      RenderLine(*prevLine, 0, prevLine->maxHeight());

      _cursorPos.set(cursorX, prevLine->maxHeight());
      updateCursor(true);
      notifyChange(ChangeState::Content);
    }
  }

  void TextArea::movedown() {
    if ((_characterPos.y()+1) == _lines.size()) {
      return;
    }
    auto nextLine = _lines[_characterPos.y() + 1];
    const auto& characters = nextLine->characters();
    int cursorX = 0;
    int charX = 0;
    for (; charX < characters.size(); ++charX) {
      auto ch = characters[charX];
      if ((cursorX + ch->getChWidth()) > _cursorPos.x()) {
        break;
      }
      cursorX += ch->getChWidth();
    }
    _characterPos.set(charX, _characterPos.y() + 1);
    int overflowY = _cursorPos.y() + nextLine->maxHeight() - (height() - 1);
    if (overflowY < 0) {
      updateCursor(cursorX, _cursorPos.y() + nextLine->maxHeight());
    } else {
      auto srcY = MAX_FONT_SIZE + overflowY;

      updateCursor(false);

      memmove(_textBuffer.buffer() + MAX_FONT_SIZE * width(), _textBuffer.buffer() + srcY * width(), (_cursorPos.y() - overflowY) * width() * _textBuffer.bytesPerPixel());
      RenderLine(*nextLine, 0, height() - 1);

      _cursorPos.set(cursorX, height() - 1);
      updateCursor(true);
      notifyChange(ChangeState::Content);
    }
  }

  void TextArea::moveleft() {
    auto curLine = _lines[_characterPos.y()];
    if (_characterPos.x() == 0) {
      return;
    }
    _characterPos.set(_characterPos.x() - 1, _characterPos.y());
    updateCursor(_cursorPos.x() - curLine->characters()[_characterPos.x()]->getChWidth(), _cursorPos.y());
  }

  void TextArea::moveright() {
    auto curLine = _lines[_characterPos.y()];
    if (_characterPos.x() == curLine->size()) {
      return;
    }
    _characterPos.set(_characterPos.x() + 1, _characterPos.y());
    updateCursor(_cursorPos.x() + curLine->characters()[_characterPos.x() - 1]->getChWidth(), _cursorPos.y());
  }

  void TextArea::clearArea(int x, int  y, uint32_t width, uint32_t height) {
    _textBuffer.fill(x, y + MAX_FONT_SIZE, width, height, backgroundColorWithAlpha());
  }

  void TextArea::onKeyboardEvent(const KeyboardEvent& event) {
    upan::mutex_guard g(_drawMutex);
    validateCursorPos();
    auto ch = event.getData().getCh();
    switch (ch) {
      case Keyboard_ENTER:
        enter();
        break;

      case Keyboard_KEY_UP:
        moveup();
        break;

      case Keyboard_KEY_DOWN:
        movedown();
        break;

      case Keyboard_KEY_LEFT:
        moveleft();
        break;

      case Keyboard_KEY_RIGHT:
        moveright();
        break;

      case Keyboard_DEL:
      case Keyboard_KEY_DEL:
        removech();
        break;

      case Keyboard_BACKSPACE:
        backspace();
        break;

      default:
        insert(ch);
        break;
    }
  }

  void TextArea::doDraw() {
    upan::mutex_guard g(_drawMutex);
    drawBuffer().copy(_textBuffer.buffer() + MAX_FONT_SIZE * _textBuffer.width(), width(), height(), true);
  }

  void TextArea::updateCursor(int x, int y) {
    upan::mutex_guard g(_drawMutex);
    updateCursor(false);
    _cursorPos.set(x, y);
    updateCursor(true);
    notifyChange(ChangeState::Content);
  }

  void TextArea::updateCursor(bool showCursor) {
    upan::mutex_guard g(_drawMutex);
    const auto color = showCursor ? _currentFGColor : _currentBGColor;
    _textBuffer.fill(_cursorPos.x() + 1, _cursorPos.y() + MAX_FONT_SIZE - 1, _currentFontSize / 2 - 1, 1, color);
  }

  void TextArea::Line::insert(int pos, Character &ch) {
    _characters.insert(pos, &ch);
    _width += ch.getChWidth();
    if (_characters.size() == 1) {
      _maxHeight = ch.getChHeight();
    } else {
      _maxHeight = upan::max(_maxHeight, ch.getChHeight());
    }
  }

  void TextArea::Line::remove(int from, int last) {
    if (from >= _characters.size()) {
      return;
    }
    _characters.erase(from, last);
    _width = 0;
    // if the line is empty then leave the maxHeight to whatever it was before
    // if the line is not empty then recalculate the maxHeight based on remaining characters
    if (!_characters.empty()) {
      _maxHeight = 0;
    }
    for (auto ch : _characters) {
      _width += ch->getChWidth();
      _maxHeight = upan::max(_maxHeight, ch->getChHeight());
    }
  }

  TextArea::CursorBlink::CursorBlink(TextArea& textArea) : upan::timer_thread(500), _textArea(textArea), _showCursorToggle(false) {
  }

  void TextArea::CursorBlink::on_timer_trigger() {
    _textArea.updateCursor(_showCursorToggle);
    _textArea.notifyChange(ChangeState::Content);
    _showCursorToggle = !_showCursorToggle;
  }
}