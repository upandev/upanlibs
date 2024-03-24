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
#include <VerticalScroller.h>

namespace upanui {
  TextArea::TextArea(int x, int y, uint32_t width, uint32_t height) : RectangleCanvas(x, y, width, height),
    _textAreaHeight(0), _currentFontSize(DEFAULT_FONT_SIZE), _currentFontType(usfn::PreloadedFonts::VGA16), _currentStyle(usfn::STYLE_REGULAR),
    _currentFGColor(0x000000), _currentBGColor(0xFFFFFF),
    _maxLineCharWidth(width - DEFAULT_SIDE_MARGIN * 2), _cursorBlinkThread(*this) {
    if (width <= DEFAULT_SIDE_MARGIN * 2) {
      throw upan::exception(XLOC, "TextArea should have a min width > 8");
    }
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

  void TextArea::init() {
    backgroundColor(0xFFFFFF | GCoreFunctions::ALPHA_MASK);
    auto curLine = new Line(_currentFontSize);
    _lines.push_back(curLine);
    _textBuffer.initLocal(width(), MAX_FONT_SIZE * 2 + height());
    _textBuffer.fill(backgroundColorWithAlpha());
    _cursorPos.set(DEFAULT_SIDE_MARGIN, curLine->lineHeight());
    _characterPos.set(0, 0);
    changeScrollHeight(curLine->lineHeight());
    _cursorBlinkThread.start();
    UIObjectImpl::captureMouseEvents(true);
  }

  usfn::Context& TextArea::getUSFNContext(usfn::PreloadedFonts fontType, uint8_t fontSize, uint16_t fontStyle) {
    const uint64_t fontContextType = fontType | fontSize << 8 | fontStyle << 16;
    auto i = _fontContexts.find(fontContextType);
    if (i != _fontContexts.end()) {
      return *(i->second);
    }
    auto context = new usfn::Context();
    context->Load(upanui::usfn::Context::GetPreloadedFont(fontType));
    context->Select(upanui::usfn::FAMILY_ANY, nullptr, fontStyle, fontSize);
    _fontContexts.insert(FontContextMap::value_type(fontContextType, context));
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
    updateCursor(false);

    Line& curLine = *_lines[_characterPos.y()];
    Line& newLine = *new Line(_currentFontSize);
    _lines.insert(_characterPos.y() + 1, &newLine);
    changeScrollHeight(newLine.lineHeight());

    for(int i = _characterPos.x(), j = 0; i < curLine.size(); ++i, ++j) {
      newLine.insert(j, *curLine.characters()[i]);
    }

    curLine.remove(_characterPos.x(), curLine.characters().size());
    curLine.wrapped(false);
    RenderLine(curLine, _characterPos.x(), _cursorPos.y());

    auto newCursorY = _cursorPos.y() + newLine.lineHeight();
    if (newCursorY < height()) {
      auto srcY = MAX_FONT_SIZE + _cursorPos.y() + 1;
      auto destY = MAX_FONT_SIZE + newCursorY + 1;
      memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + srcY * width(), (height() - newCursorY - 1) * width() * _textBuffer.bytesPerPixel());
      RenderLine(newLine, 0, newCursorY);
    } else {
      auto moveY = newCursorY - (height() - 1);
      auto srcY = MAX_FONT_SIZE + moveY;
      auto destY = MAX_FONT_SIZE;
      memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + srcY * width(),
              (_cursorPos.y() - moveY) * width() * _textBuffer.bytesPerPixel());
      newCursorY = height() - 1;
      RenderLine(newLine, 0, newCursorY);
    }

    _characterPos.set(0, _characterPos.y() + 1);
    _cursorPos.set(DEFAULT_SIDE_MARGIN, newCursorY);
    updateCursor(true);
  }

  void TextArea::insert(uint16_t ch) {
    if (is_command_key(ch)) {
      return;
    }

    auto newCh = new Character(ch, _currentFontSize, _currentFontType, _currentStyle, _currentFGColor, _currentBGColor);

    auto line = _lines[_characterPos.y()];
    auto origLineCount = _lines.size();

    Characters characters;
    characters.push_back(newCh);
    insert(*line, _characterPos.x(), _characterPos.y(), characters);

    auto hasNewLine = _lines.size() != origLineCount;

    if (hasNewLine) {
      auto srcCursorY = _cursorPos.y();
      auto destCursorY = _cursorPos.y();
      for (int i = _characterPos.y() + 1; i < _lines.size(); ++i) {
        auto l = _lines[i];
        if (l->wrapped()) {
          srcCursorY += l->lineHeight();
        } else {
          destCursorY = srcCursorY + l->lineHeight();
          break;
        }
      }

      if (destCursorY < height()) {
        auto srcY = MAX_FONT_SIZE + srcCursorY + 1;
        auto destY = MAX_FONT_SIZE + destCursorY + 1;
        memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + srcY * width(),
                (height() - destCursorY - 1) * width() * _textBuffer.bytesPerPixel());
      }
    }

    auto cursorY = _cursorPos.y();
    for(int i = _characterPos.y(); i < _lines.size(); ++i) {
      RenderLine(*_lines[i], 0, cursorY);
      if (!_lines[i]->wrapped()) { break; }
      if (cursorY >= height()) { break; }
      cursorY += _lines[i]->lineHeight();
    }

    if (line->size() == _characterPos.x()) {
      //This means the new character inserted has moved to the next line
      _characterPos.set(1, _characterPos.y());
      updateCursor(newCh->getChWidth() + DEFAULT_SIDE_MARGIN, _cursorPos.y());
      movedown();
    } else {
      _characterPos.set(_characterPos.x() + 1, _characterPos.y());
      updateCursor(_cursorPos.x() + newCh->getChWidth(), _cursorPos.y());
    }
  }

  void TextArea::insert(TextArea::Line& line, int lineX, int lineY, const TextArea::Characters& characters) {
    for(int i = 0; i < characters.size(); ++i) {
      line.insert(lineX + i, *characters[i]);
    }

    Characters wrapCharacters;
    while (line.width() > _maxLineCharWidth) {
      auto ch = line.characters(line.size() - 1);
      line.remove(line.size() - 1, line.size());
      wrapCharacters.push_back(ch);
    }

    if (!wrapCharacters.empty()) {
      lineY += 1;
      if (line.wrapped()) {
        auto nextLine = _lines[lineY];
        insert(*nextLine, 0, lineY, wrapCharacters);
      } else {
        line.wrapped(true);
        auto newLine = new Line(_currentFontSize);
        _lines.insert(lineY, newLine);
        changeScrollHeight(newLine->lineHeight());
        insert(*newLine, 0, lineY, wrapCharacters);
      }
    }
  }

  void TextArea::wrapremovech(int x, int y, int& deletedLine) {
    auto line = _lines[y];
    line->remove(x, x + 1);

    if (!line->wrapped()) {
      return;
    }

    auto ny = y + 1;
    if (ny < _lines.size()) {
      auto nextLine = _lines[ny];
      int availWidth = (int)_maxLineCharWidth - line->width();
      bool deletedFromNextLine = false;
      while (nextLine->size() > 0) {
        auto ch = nextLine->characters(0);
        if (ch->getChWidth() < availWidth) {
          line->insert(line->size(), *ch);
          availWidth -= ch->getChWidth();
          wrapremovech(0, ny, deletedLine);
          deletedFromNextLine = true;
        } else {
          break;
        }
      }
      //deleting when cursor is at the end of current line which is full - then we need to remove the first char from next line
      if (!deletedFromNextLine && nextLine->size() > 0 && x == line->size()) {
        wrapremovech(0, ny, deletedLine);
      }
      if (nextLine->size() == 0) {
        line->wrapped(false);
        deletedLine = ny;
      }
    }
  }

  void TextArea::lineremovech(const int y, const int baseY) {
    auto line = _lines[y];
    auto visibleBaseY = baseY - line->lineHeight();
    auto insideCanvas = visibleBaseY < height();

    if (y < (_lines.size() - 1) && insideCanvas) {
      int copyHeight = 0;
      int lastLineIndex = -1;
      auto lastLineBaseCursorY = baseY;

      for (int i = y + 1; i < _lines.size(); ++i) {
        lastLineBaseCursorY += _lines[i]->lineHeight();
        if (lastLineBaseCursorY >= height()) {
          lastLineIndex = i;
          break;
        }
        copyHeight += _lines[i]->lineHeight();
      }

      const auto destY = MAX_FONT_SIZE + visibleBaseY + 1;
      const auto srcY = MAX_FONT_SIZE + baseY;

      memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + (srcY + 1) * width(), copyHeight * width() * _textBuffer.bytesPerPixel());

      const int destYOnCanvas = destY + copyHeight - MAX_FONT_SIZE;
      if (copyHeight > 0) {
        clearArea(0, destYOnCanvas, width(), height() - destYOnCanvas);
      }

      for (int lastLineBaseY = destYOnCanvas; lastLineBaseY < height() && lastLineIndex != -1 && lastLineIndex < _lines.size();) {
        auto lastLine = _lines[lastLineIndex];
        RenderLine(*lastLine, 0, lastLineBaseY + lastLine->lineHeight());
        lastLineBaseY += lastLine->lineHeight();
        ++lastLineIndex;
      }

      _lines.erase(y, y + 1);
      changeScrollHeight(-line->lineHeight());
    } else if (_characterPos.y() < y) { //its a no-op if deleting the line where character cursor is
      if (insideCanvas) {
        clearArea(0, visibleBaseY + 1, width(), line->lineHeight());
      }
      _lines.erase(y, y + 1);
      changeScrollHeight(-line->lineHeight());
    }
  }

  void TextArea::removech() {
    auto line = _lines[_characterPos.y()];
    if (!line->wrapped() && _characterPos.x() == line->characters().size() && _characterPos.x() > 0) {
      line->wrapped(true);
    }
    if (line->wrapped()) {
      int deletedLine = -1;
      wrapremovech(_characterPos.x(), _characterPos.y(), deletedLine);
      int baseY = _cursorPos.y() - line->lineHeight();
      for (int i = _characterPos.y(); i < _lines.size(); ++i) {
        auto l = _lines[i];
        if (baseY > height()) {
          break;
        }
        baseY += l->lineHeight();
        RenderLine(*l, 0, baseY);
        if (!l->wrapped()) {
          break;
        }
      }

      if (deletedLine > 0) {
        int baseY = _cursorPos.y() - line->lineHeight();
        for (int i = _characterPos.y(); i <= deletedLine; ++i) {
          baseY += _lines[i]->lineHeight();
        }
        lineremovech(deletedLine, baseY);
      }
    } else {
      if (_characterPos.x() == line->characters().size()) {
        if (_characterPos.x() == 0) {
          lineremovech(_characterPos.y(), _cursorPos.y());
        }
      } else {
        line->remove(_characterPos.x(), _characterPos.x() + 1);
        RenderLine(*line, _characterPos.x(), _cursorPos.y());
      }
    }
  }

  void TextArea::backspace() {
    if (_characterPos.x() == 0) {
      if (_characterPos.y() == 0) {
        return;
      }
    }
    moveleft();
    removech();
  }

  void TextArea::moveup() {
    if (_characterPos.y() == 0) {
      return;
    }
    auto curLine = _lines[_characterPos.y()];
    auto prevLine = _lines[_characterPos.y() - 1];

    const auto& characters = prevLine->characters();
    auto cursorY = _cursorPos.y() - curLine->lineHeight();
    int cursorX = DEFAULT_SIDE_MARGIN;
    int charX = 0;
    for (; charX < characters.size(); ++charX) {
      auto ch = characters[charX];
      if ((cursorX + ch->getChWidth()) > _cursorPos.x()) {
        break;
      }
      cursorX += ch->getChWidth();
    }
    _characterPos.set(charX, _characterPos.y() - 1);

    if (cursorY >= prevLine->lineHeight()) {
      updateCursor(cursorX, cursorY);
    } else {
      auto srcY = MAX_FONT_SIZE + cursorY;
      auto destY = MAX_FONT_SIZE + prevLine->lineHeight();

      updateCursor(false);

      memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + srcY * width(), (height() - cursorY) * width() * _textBuffer.bytesPerPixel());
      RenderLine(*prevLine, 0, prevLine->lineHeight());

      _cursorPos.set(cursorX, prevLine->lineHeight());
      updateCursor(true);
    }
  }

  void TextArea::movedown() {
    if ((_characterPos.y()+1) == _lines.size()) {
      return;
    }
    auto nextLine = _lines[_characterPos.y() + 1];
    const auto& characters = nextLine->characters();
    int cursorX = DEFAULT_SIDE_MARGIN;
    int charX = 0;
    for (; charX < characters.size(); ++charX) {
      auto ch = characters[charX];
      if ((cursorX + ch->getChWidth()) > _cursorPos.x()) {
        break;
      }
      cursorX += ch->getChWidth();
    }
    _characterPos.set(charX, _characterPos.y() + 1);
    int overflowY = _cursorPos.y() + nextLine->lineHeight() - (height() - 1);
    if (overflowY < 0) {
      updateCursor(cursorX, _cursorPos.y() + nextLine->lineHeight());
    } else {
      auto srcY = MAX_FONT_SIZE + overflowY;

      updateCursor(false);

      memmove(_textBuffer.buffer() + MAX_FONT_SIZE * width(), _textBuffer.buffer() + srcY * width(), (_cursorPos.y() - overflowY) * width() * _textBuffer.bytesPerPixel());
      RenderLine(*nextLine, 0, height() - 1);

      _cursorPos.set(cursorX, height() - 1);
      updateCursor(true);
    }
  }

  void TextArea::moveleft() {
    if (_characterPos.x() == 0) {
      if (_characterPos.y() == 0) {
        return;
      }
      moveup();
      moveend();
      return;
    }
    _characterPos.set(_characterPos.x() - 1, _characterPos.y());
    auto curLine = _lines[_characterPos.y()];
    updateCursor(_cursorPos.x() - curLine->characters()[_characterPos.x()]->getChWidth(), _cursorPos.y());
  }

  void TextArea::moveright() {
    auto curLine = _lines[_characterPos.y()];
    if (_characterPos.x() == curLine->size()) {
      if ((_characterPos.y()+1) == _lines.size()) {
        return;
      }
      movehome();
      movedown();
      return;
    }
    _characterPos.set(_characterPos.x() + 1, _characterPos.y());
    updateCursor(_cursorPos.x() + curLine->characters()[_characterPos.x() - 1]->getChWidth(), _cursorPos.y());
  }

  void TextArea::movehome() {
    if (_characterPos.x() == 0) {
      return;
    }
    _characterPos.set(0, _characterPos.y());
    updateCursor(DEFAULT_SIDE_MARGIN, _cursorPos.y());
  }

  void TextArea::moveend() {
    auto curLine = _lines[_characterPos.y()];
    if (_characterPos.x() == curLine->size()) {
      return;
    }

    auto cursorX = _cursorPos.x();
    auto charX = _characterPos.x();
    while(charX < curLine->size()) {
      cursorX += curLine->characters()[charX]->getChWidth();
      ++charX;
    }
    _characterPos.set(charX, _characterPos.y());
    updateCursor(cursorX, _cursorPos.y());
  }

  void TextArea::clearArea(int x, int  y, uint32_t width, uint32_t height) {
    _textBuffer.fill(x, y + MAX_FONT_SIZE, width, height, backgroundColorWithAlpha());
  }

  void TextArea::fillCharacterBG(int x, int  y, uint32_t height, const Character& ch) {
    _textBuffer.fill(x, y + MAX_FONT_SIZE, ch.getChWidth(), height, ch.getBgColor() | (backgroundColorAlpha() << 24));
  }

  void TextArea::onKeyboardEvent(const KeyboardEvent& event) {
    upan::mutex_guard g(_drawMutex);
    validateCursorPos();
    auto ch = event.getData().getRch();
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

      case Keyboard_KEY_HOME:
        movehome();
        break;

      case Keyboard_KEY_END:
        moveend();
        break;

      case Keyboard_CTRL_B:
        if (_currentStyle & usfn::STYLE_BOLD) {
          _currentStyle &= ~(usfn::STYLE_BOLD);
        } else {
          _currentStyle |= usfn::STYLE_BOLD;
        }
        break;

      case Keyboard_CTRL_I:
        if (_currentStyle & usfn::STYLE_ITALIC) {
          _currentStyle &= ~(usfn::STYLE_ITALIC);
        } else {
          _currentStyle |= usfn::STYLE_ITALIC;
        }
        break;

      case Keyboard_CTRL_W:
        _currentBGColor = 0xFFFFFF;
        break;

      case Keyboard_CTRL_G:
        _currentBGColor = 0x00FF00;
        break;

      case Keyboard_CTRL_A:
        _currentFontSize *= 2;
        if (_currentFontSize > 80) {
          _currentFontSize = 80;
        }
        break;

      case Keyboard_CTRL_Q:
        _currentFontSize /= 2;
        if (_currentFontSize < 16)
          _currentFontSize = 16;
        break;

      default:
        insert(ch);
        break;
    }
    notifyChange(ChangeState::Content);
  }

  void TextArea::doDraw() {
    upan::mutex_guard g(_drawMutex);
    drawBuffer().copy(_textBuffer.buffer() + MAX_FONT_SIZE * _textBuffer.width(), width(), height(), true);
  }

  void TextArea::RenderLine(const Line& line, int charX, int baseDrawY) {
    auto topY = baseDrawY - line.lineHeight();
    if (topY > height()) {
      return;
    }

    auto drawX = DEFAULT_SIDE_MARGIN;
    auto& characters = line.characters();
    for(int i = 0; i < charX; ++i) {
      drawX += characters[i]->getChWidth();
    }
    clearArea(drawX, topY, width() - drawX, line.lineHeight() + 1);

    for(int i = charX; i < characters.size(); ++i) {
      const auto& ch = *characters[i];
      usfn::FrameBuffer buf = {
          .ptr = (uint8_t*)_textBuffer.buffer(),
          .w = (int16_t)_textBuffer.width(),
          .h = (int16_t)_textBuffer.height(),
          .p = (uint16_t)_textBuffer.pitch(),
          .x = (int16_t)drawX,
          .y = (int16_t)(baseDrawY + MAX_FONT_SIZE - 1 - ch.getChHeight()),
          .fg = _currentFGColor | GCoreFunctions::ALPHA_MASK,
          .bg = backgroundColor() | GCoreFunctions::ALPHA_MASK
      };

      fillCharacterBG(drawX, topY, line.lineHeight() + 1, ch);

      char str[2] = { (char)ch.getCh(), '\0'};
      auto& sfnContext = getUSFNContext(ch.getFontType(), ch.getFontSize(), ch.getStyle());
      sfnContext.RenderText(buf, str, true, false);

      drawX += ch.getChWidth();
    }
  }

  void TextArea::updateCursor(int x, int y) {
    upan::mutex_guard g(_drawMutex);
    updateCursor(false);
    _cursorPos.set(x, y);
    updateCursor(true);
  }

  void TextArea::updateCursor(bool showCursor) {
    upan::mutex_guard g(_drawMutex);
    const auto color = (showCursor ? _currentFGColor : _currentBGColor) | GCoreFunctions::ALPHA_MASK;
    _textBuffer.fill(_cursorPos.x() + 1, _cursorPos.y() + MAX_FONT_SIZE - 1, _currentFontSize / 2 - 1, 1, color);
  }

  uint32_t TextArea::scrollHeight() const {
    if (_textAreaHeight < height()) {
      return height();
    } else {
      return _textAreaHeight;
    }
  }

  void TextArea::changeScrollHeight(int delta) {
    auto oldScrollHeight = scrollHeight();

    int newHeight = _textAreaHeight + delta;
    if (newHeight < 0) {
      _textAreaHeight = 0;
    } else {
      _textAreaHeight = newHeight;
    }

    auto newScrollHeight = scrollHeight();

    if (oldScrollHeight != newScrollHeight) {
      getVerticalScroller().ifPresent([](VerticalScroller &verticalScroller) {
        verticalScroller.caliberateScrollbar();
      });
    }
  }

  void TextArea::vscroll(int rows, int scrollableHeight) {
    int newY = y() + rows;
    if (newY > 0) {
      newY = 0;
    } else {
      int h = (int)scrollHeight() + newY;
      if (h < scrollableHeight) {
        newY = scrollableHeight - (int)scrollHeight();
      }
    }
    y(newY);
  }

  void TextArea::Line::insert(int pos, Character &ch) {
    _characters.insert(pos, &ch);
    _width += ch.getChWidth();
    if (_characters.size() == 1) {
      _maxChHeight = ch.getChHeight();
    } else {
      _maxChHeight = upan::max(_maxChHeight, ch.getChHeight());
    }
  }

  void TextArea::Line::remove(int from, int last) {
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
    for (auto ch : _characters) {
      _width += ch->getChWidth();
      _maxChHeight = upan::max(_maxChHeight, ch->getChHeight());
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