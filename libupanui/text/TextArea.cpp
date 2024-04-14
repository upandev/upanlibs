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
  TextArea::TextArea(int x, int y, int width, int height) : RectangleCanvas(x, y, width, height),
    _scrollY(0), _scrollHeight(0),
    _currentFontSize(DEFAULT_FONT_SIZE), _currentFontType(usfn::PreloadedFonts::VGA16), _currentStyle(usfn::STYLE_REGULAR),
    _currentFGColor(DEFAULT_FG_COLOR), _currentBGColor(DEFAULT_BG_COLOR),
    _maxLineCharWidth(width - DEFAULT_SIDE_MARGIN * 2), _cursorBlinkThread(*this), _mouseHandler(nullptr) {
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
    _selectedArea.setPresent(false);
    _textBuffer.initLocal(width(), MAX_FONT_SIZE * 2 + height());
    _textBuffer.fill(backgroundColorWithAlpha());
    //cursor y is within the character pixel block.
    //the character rendering should ensure that the character is drawn with certain bottom margin
    //for the cursor to be visible at the last pixel row of the character block.
    _cursorPos.set(DEFAULT_SIDE_MARGIN, curLine->lineHeight() - 1);
    _characterPos.set(0, 0);
    changeScrollHeight(curLine->lineHeight());
    _cursorBlinkThread.start();

    UIObjectImpl::captureMouseEvents(true);
    _mouseHandler.reset(new TextAreaMouseHandler(*this));
    registerMouseEventHandler(*_mouseHandler);
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

  void TextArea::scrollToCursor() {
    int rows;
    if (_cursorPos.y() < 0) {
      rows =  -(_cursorPos.y() - _lines[_characterPos.y()]->lineHeight() + 1);
    } else if (_cursorPos.y() >= height()) {
      rows =  -(_cursorPos.y() - height() + 1);
    } else {
      return;
    }

    vscroll(rows, height());
    _scrollerChanges.capture(false, true, _scrollY);
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

    Line* prevLine = (_characterPos.y() > 0) ? _lines[_characterPos.y() - 1] : nullptr;
    if (_characterPos.x() == 0) {
      if (prevLine && prevLine->wrapped()) {
        prevLine->wrapped(false);
        return;
      }
    }

    Line& newLine = *new Line(_currentFontSize);
    _lines.insert(_characterPos.y() + 1, &newLine);

    for(int i = _characterPos.x(), j = 0; i < curLine.size(); ++i, ++j) {
      newLine.insert(j, curLine.characters()[i]);
    }

    newLine.wrapped(curLine.wrapped());
    curLine.remove(_characterPos.x(), curLine.characters().size());
    curLine.wrapped(false);
    renderLine(curLine, _characterPos.x(), _characterPos.y(), _cursorPos.y());

    auto newCursorY = _cursorPos.y() + newLine.lineHeight();
    if (newCursorY < height()) {
      auto srcY = MAX_FONT_SIZE + _cursorPos.y() + 1;
      auto destY = MAX_FONT_SIZE + newCursorY + 1;
      memmove(_textBuffer.buffer() + destY * width(),
              _textBuffer.buffer() + srcY * width(),
              (height() - newCursorY - 1) * width() * _textBuffer.bytesPerPixel());
      renderLine(newLine, 0, _characterPos.y() + 1, newCursorY);
    }

    changeScrollHeight(newLine.lineHeight());
    bool unwrapNextLine = _characterPos.x() > 0 && newLine.wrapped();
    movehome();
    movedown();
    if (unwrapNextLine) {
      moveend();
      removech();
      movehome();
    }
  }

  void TextArea::selectAll() {
    _selectedArea.setPresent(false);

    if (_lines.empty()) {
      return;
    }

    if (_lines.size() == 1 && _lines[0]->size() == 0) {
      return;
    }

    Position p1, p2;
    p1.set(0, 0);
    p2.set(_lines[_lines.size() - 1]->size(), _lines.size() - 1);
    _selectedArea.setPresent(true);
    _selectedArea.setPivot(p1);
    _selectedArea.setRange(p1, p2);

    renderLineRange(p1, p2);
  }

  void TextArea::cutSelection() {
    copySelection();
    deleteSelectedArea();
  }

  void TextArea::copySelection() {
    _copyBuffer.clear();

    if (!_selectedArea.isPresent()) {
      return;
    }

    if (_selectedArea.p1().y() == _selectedArea.p2().y()) {
      auto line = _lines[_selectedArea.p1().y()];

      for(int x = _selectedArea.p1().x(); x < _selectedArea.p2().x() && x < line->size(); ++x) {
        _copyBuffer.push_back(line->characters(x));
      }
    } else {
      const Character newLineCharacter(Keyboard_ENTER, _currentFontSize, _currentFontType, _currentStyle, _currentFGColor, _currentBGColor);
      {
        auto line1 = _lines[_selectedArea.p1().y()];
        for (int x = _selectedArea.p1().x(); x < line1->size(); ++x) {
          _copyBuffer.push_back(line1->characters(x));
        }
        if (!line1->wrapped()) {
          _copyBuffer.push_back(newLineCharacter);
        }
      }

      {
        for (int y = _selectedArea.p1().y() + 1; y < _selectedArea.p2().y(); ++y) {
          auto line = _lines[y];
          for (const auto &ch : line->characters()) {
            _copyBuffer.push_back(ch);
          }
          if (!line->wrapped()) {
            _copyBuffer.push_back(newLineCharacter);
          }
        }
      }

      {
        auto line2 = _lines[_selectedArea.p2().y()];
        for (int x = 0; x < _selectedArea.p2().x() && x < line2->size(); ++x) {
          _copyBuffer.push_back(line2->characters(x));
        }
      }
    }
  }

  void TextArea::paste() {
    for(const auto& ch : _copyBuffer) {
      insert(ch);
    }
  }

  void TextArea::insert(uint8_t ch) {
    if (!isInsertableKey(ch)) {
      return;
    }

    deleteSelectedArea();
    Character newCh(ch, _currentFontSize, _currentFontType, _currentStyle, _currentFGColor, _currentBGColor);
    insert(newCh);
  }

  void TextArea::insert(const Character& ch) {
    if (ch.getCh() == Keyboard_ENTER) {
      enter();
      return;
    }

    auto line = _lines[_characterPos.y()];
    auto origLineCount = _lines.size();

    Characters characters;
    characters.push_back(ch);
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
      renderLine(*_lines[i], 0, i, cursorY);
      if (!_lines[i]->wrapped()) { break; }
      if (cursorY >= height()) { break; }
      cursorY += _lines[i]->lineHeight();
    }

    if (line->size() == _characterPos.x()) {
      //This means the new character inserted has moved to the next line
      updateCursorPosition(1, _characterPos.y(), ch.getChWidth() + DEFAULT_SIDE_MARGIN, _cursorPos.y());
      movedown();
    } else {
      updateCursorPosition(_characterPos.x() + 1, _characterPos.y(), _cursorPos.x() + ch.getChWidth(), _cursorPos.y());
    }
  }

  void TextArea::insert(TextArea::Line& line, int lineX, int lineY, const TextArea::Characters& characters) {
    for(int i = 0; i < characters.size(); ++i) {
      line.insert(lineX + i, characters[i]);
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
      int availWidth = _maxLineCharWidth - line->width();
      bool deletedFromNextLine = false;
      while (nextLine->size() > 0) {
        auto ch = nextLine->characters(0);
        if (ch.getChWidth() < availWidth) {
          line->insert(line->size(), ch);
          availWidth -= ch.getChWidth();
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

  void TextArea::lineremovech(const int y) {
    const int baseY = getLineBaseY(y);
    auto line = _lines[y];
    auto visibleBaseY = baseY - line->lineHeight() + 1;
    auto insideCanvas = visibleBaseY < height();

    if (y > 0) {
      auto prevLine = _lines[y - 1];
      if (prevLine->wrapped()) {
        prevLine->wrapped(line->wrapped());
      }
    }

    if (y < (_lines.size() - 1) && insideCanvas) {
      int copyHeight = 0;
      int lastLineIndex = -1;
      auto lastLineBaseCursorY = baseY;

      for (int i = y + 1; i < _lines.size(); ++i) {
        lastLineBaseCursorY += _lines[i]->lineHeight();
        if (lastLineBaseCursorY >= (height() - 1)) {
          lastLineIndex = i;
          break;
        }
        copyHeight += _lines[i]->lineHeight();
      }

      const auto destY = MAX_FONT_SIZE + visibleBaseY;
      const auto srcY = MAX_FONT_SIZE + baseY + 1;

      memmove(_textBuffer.buffer() + destY * width(),
              _textBuffer.buffer() + srcY * width(),
              copyHeight * width() * _textBuffer.bytesPerPixel());

      const int destYOnCanvas = destY + copyHeight - MAX_FONT_SIZE;
      if (copyHeight > 0) {
        clearArea(0, destYOnCanvas, width(), height() - destYOnCanvas);
      }

      for (int lastLineTopY = destYOnCanvas; lastLineTopY < height() && lastLineIndex != -1 && lastLineIndex < _lines.size();) {
        auto lastLine = _lines[lastLineIndex];
        renderLine(*lastLine, 0, lastLineIndex, lastLineTopY + lastLine->lineHeight() - 1);
        lastLineTopY += lastLine->lineHeight();
        ++lastLineIndex;
      }

      _lines.erase(y, y + 1);
      changeScrollHeight(-line->lineHeight());
      delete line;
    } else if (_characterPos.y() < y) { //its a no-op if deleting the line where character cursor is
      if (insideCanvas) {
        clearArea(0, visibleBaseY, width(), line->lineHeight());
      }
      _lines.erase(y, y + 1);
      changeScrollHeight(-line->lineHeight());
      delete line;
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
        if (baseY >= ((int)height() - 1)) {
          break;
        }
        baseY += l->lineHeight();
        renderLine(*l, 0, i, baseY);
        if (!l->wrapped()) {
          break;
        }
      }

      if (deletedLine > 0) {
        lineremovech(deletedLine);
      }
    } else {
      if (_characterPos.x() == line->characters().size()) {
        if (_characterPos.x() == 0) {
          lineremovech(_characterPos.y());
        }
      } else {
        line->remove(_characterPos.x(), _characterPos.x() + 1);
        renderLine(*line, _characterPos.x(), _characterPos.y(), _cursorPos.y());
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
      if ((cursorX + ch.getChWidth()) > _cursorPos.x()) {
        break;
      }
      cursorX += ch.getChWidth();
    }

    if (cursorY >= (prevLine->lineHeight() - 1)) {
      updateCursorPosition(charX, _characterPos.y() - 1, cursorX, cursorY);
    } else {
      int destCursorY = prevLine->lineHeight() - 1;
      auto srcY = MAX_FONT_SIZE + cursorY + 1;
      auto destY = MAX_FONT_SIZE + destCursorY + 1;

      updateCursor(false);
      _characterPos.set(charX, _characterPos.y() - 1);

      memmove(_textBuffer.buffer() + destY * width(),
              _textBuffer.buffer() + srcY * width(),
              (height() - destCursorY - 1) * width() * _textBuffer.bytesPerPixel());
      renderLine(*prevLine, 0, _characterPos.y(), destCursorY);

      _cursorPos.set(cursorX, destCursorY);
      updateCursor(true);

      updateScrollY(-(destCursorY - cursorY));
      _scrollerChanges.capture(false, true, _scrollY);
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
      if ((cursorX + ch.getChWidth()) > _cursorPos.x()) {
        break;
      }
      cursorX += ch.getChWidth();
    }

    int overflowY = _cursorPos.y() + nextLine->lineHeight() - height();
    if (overflowY < 0) {
      updateCursorPosition(charX, _characterPos.y() + 1, cursorX, _cursorPos.y() + nextLine->lineHeight());
    } else {
      auto srcY = MAX_FONT_SIZE + overflowY + 1;
      int destY = MAX_FONT_SIZE;

      updateCursor(false);
      _characterPos.set(charX, _characterPos.y() + 1);

      memmove(_textBuffer.buffer() + destY * width(), _textBuffer.buffer() + srcY * width(), (_cursorPos.y() - overflowY) * width() * _textBuffer.bytesPerPixel());
      renderLine(*nextLine, 0, _characterPos.y(), height() - 1);

      _cursorPos.set(cursorX, height() - 1);
      updateCursor(true);

      updateScrollY(overflowY + 1);
      _scrollerChanges.capture(false, true, _scrollY);
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

    auto curLine = _lines[_characterPos.y()];
    updateCursorPosition(_characterPos.x() - 1, _characterPos.y(),
                         _cursorPos.x() - curLine->characters()[_characterPos.x() - 1].getChWidth(), _cursorPos.y());
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

    updateCursorPosition(_characterPos.x() + 1, _characterPos.y(),
                         _cursorPos.x() + curLine->characters()[_characterPos.x()].getChWidth(), _cursorPos.y());
  }

  void TextArea::movehome() {
    if (_characterPos.x() == 0) {
      return;
    }

    updateCursorPosition(0, _characterPos.y(), DEFAULT_SIDE_MARGIN, _cursorPos.y());
  }

  void TextArea::moveend() {
    auto curLine = _lines[_characterPos.y()];
    if (_characterPos.x() == curLine->size()) {
      return;
    }

    auto cursorX = _cursorPos.x();
    auto charX = _characterPos.x();
    while(charX < curLine->size()) {
      cursorX += curLine->characters()[charX].getChWidth();
      ++charX;
    }

    updateCursorPosition(charX, _characterPos.y(), cursorX, _cursorPos.y());
  }

  void TextArea::clearArea(int x, int  y, int width, int height) {
    _textBuffer.fill(x, y + MAX_FONT_SIZE, width, height, backgroundColorWithAlpha());
  }

  uint32_t TextArea::getChBgColor(int cx, int cy, const Character& ch) const {
    const static uint32_t selectedAreaBGColor = 0x81B3F0 | GCoreFunctions::ALPHA_MASK;
    return _selectedArea.isPresent() && _selectedArea.inRange(cx, cy) ? selectedAreaBGColor : (ch.getBgColor() | (backgroundColorAlpha() << 24));
  }

  void TextArea::fillCharacterBG(int x, int y, int cx, int cy, int height, const Character &ch) {
    _textBuffer.fill(x, y + MAX_FONT_SIZE, ch.getChWidth(), height, getChBgColor(cx, cy, ch));
  }

  void TextArea::unselectArea() {
    //unselect the selected area
    _selectedArea.setPresent(false);
    renderLineRange(_selectedArea.p1(), _selectedArea.p2());
  }

  void TextArea::updateSelectedArea(bool isSelectionOn, bool isSelectKey, const Position &prevCharPos) {
    if (!isSelectionOn) {
      if (_selectedArea.isPresent() && isSelectKey) {
        unselectArea();
      }
    } else {
      if (!isSelectKey) {
        return;
      }

      const Position newCharPos = _characterPos;
      Position rp1, rp2;
      if (_selectedArea.isPresent()) {
        _selectedArea.setRange(_selectedArea.pivot(), newCharPos);
        if (prevCharPos <= newCharPos) {
          rp1 = prevCharPos;
          rp2 = newCharPos;
        } else {
          rp1 = newCharPos;
          rp2 = prevCharPos;
        }
      } else {
        _selectedArea.setPresent(true);
        _selectedArea.setPivot(prevCharPos);
        _selectedArea.setRange(prevCharPos, newCharPos);
        rp1 = _selectedArea.p1();
        rp2 = _selectedArea.p2();
      }

      renderLineRange(rp1, rp2);
    }
  }

  void TextArea::ScrollerChanges::capture(bool calibrate, bool adjustScrollY, int scrollY) {
    if (!_calibrate) _calibrate = calibrate;
    if (!_adjustScrollY) _adjustScrollY = adjustScrollY;
    _scrollY = scrollY;
  }

  void TextArea::ScrollerChanges::apply(VerticalScroller& verticalScroller) {
    if (_calibrate) {
      verticalScroller.caliberateScrollbar();
    }

    if (_adjustScrollY) {
      verticalScroller.updateScrollPosition(_scrollY);
    }
    _calibrate = _adjustScrollY = false;
  }

  void TextArea::onKeyboardEvent(const KeyboardEvent& event) {
    processKeyboardEvent(event);
    getVerticalScroller().ifPresent([this](VerticalScroller& verticalScroller) { _scrollerChanges.apply(verticalScroller); });
  }

  void TextArea::processKeyboardEvent(const KeyboardEvent& event) {
    upan::mutex_guard g(_drawMutex);
    validateCursorPos();

    const Position prevCharPos = _characterPos;
    auto ch = event.getData().getRch();

    if (isSelectKey(ch) || isTextModifyKey(ch)) {
      scrollToCursor();
    }

    switch (ch) {
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

      case Keyboard_KEY_HOME:
        movehome();
        break;

      case Keyboard_KEY_END:
        moveend();
        break;

      case Keyboard_DEL:
      case Keyboard_KEY_DEL:
        if (_selectedArea.isPresent()) {
          deleteSelectedArea();
        } else {
          removech();
        }
        break;

      case Keyboard_BACKSPACE:
        if (_selectedArea.isPresent()) {
          deleteSelectedArea();
        } else {
          backspace();
        }
        break;

      case Keyboard_CTRL_A:
        selectAll();
        break;

      case Keyboard_CTRL_X:
        cutSelection();
        break;

      case Keyboard_CTRL_C:
        copySelection();
        break;

      case Keyboard_CTRL_V:
        paste();
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

      default:
        insert(ch);
        break;
    }

    updateSelectedArea(event.getData().isShiftPressed(), isSelectKey(ch), prevCharPos);
    notifyChange(ChangeState::Content);
  }

  void TextArea::doDraw() {
    upan::mutex_guard g(_drawMutex);
    drawBuffer().copy(_textBuffer.buffer() + MAX_FONT_SIZE * _textBuffer.width(), width(), height(), true);
  }

  void TextArea::renderLine(const Line &line, int charX, int charY, int baseDrawY) {
    int topY = baseDrawY - line.lineHeight() + 1;
    if (topY >= height() || baseDrawY < 0) {
      return;
    }

    auto drawX = DEFAULT_SIDE_MARGIN;
    auto& characters = line.characters();
    for(int i = 0; i < charX; ++i) {
      drawX += characters[i].getChWidth();
    }
    clearArea(drawX, topY, width() - drawX, line.lineHeight());

    for(int i = charX; i < characters.size(); ++i) {
      const auto& ch = characters[i];
      usfn::FrameBuffer buf = {
          .ptr = (uint8_t*)_textBuffer.buffer(),
          .w = (int16_t)_textBuffer.width(),
          .h = (int16_t)_textBuffer.height(),
          .p = (uint16_t)_textBuffer.pitch(),
          .x = (int16_t)drawX,
              //character height is less than the line height because line height is padded with line spacing
              //therefore, it is not required do a +1 on baseDrawY unlike how it is done while calculating topY using line height
              //Remember, baseDrawY is same as the cursor Y which is the bottom Y of the character block
          .y = (int16_t)(baseDrawY + MAX_FONT_SIZE - ch.getChHeight()),
          .fg = _currentFGColor | GCoreFunctions::ALPHA_MASK,
          .bg = backgroundColor() | GCoreFunctions::ALPHA_MASK
      };

      fillCharacterBG(drawX, topY, i, charY, line.lineHeight(), ch);

      char str[2] = { (char)ch.getCh(), '\0'};
      auto& sfnContext = getUSFNContext(ch.getFontType(), ch.getFontSize(), ch.getStyle());
      sfnContext.RenderText(buf, str, true, false);

      drawX += ch.getChWidth();
    }
  }

  void TextArea::updateCursorPosition(int charPosX, int charPosY, int cursorPosX, int cursorPosY) {
    upan::mutex_guard g(_drawMutex);
    updateCursor(false);
    _characterPos.set(charPosX, charPosY);
    _cursorPos.set(cursorPosX, cursorPosY);
    updateCursor(true);
  }

  void TextArea::updateCursor(bool showCursor) {
    upan::mutex_guard g(_drawMutex);

    auto color = (showCursor ? DEFAULT_FG_COLOR : DEFAULT_BG_COLOR);
    auto& line = *_lines[_characterPos.y()];
    if (_characterPos.x() < line.characters().size()) {
      const auto ch = line.characters(_characterPos.x());
      color = (showCursor ? ch.getFgColor() : getChBgColor(_characterPos.x(), _characterPos.y(), ch));
    }
    color |= GCoreFunctions::ALPHA_MASK;

    if (_cursorPos.y() >= 0 && _cursorPos.y() < height()) {
      _textBuffer.fill(_cursorPos.x() + 1, _cursorPos.y() + MAX_FONT_SIZE - 1, _currentFontSize / 2 - 1, 1, color);
    }
  }

  int TextArea::updateScrollY(int sy) {
    int newScrollY = _scrollY + sy;
    if (newScrollY < 0) {
      newScrollY = 0;
    } else if (newScrollY >= _scrollHeight) {
      newScrollY = _scrollHeight - 1;
    }
    const int rows = newScrollY - _scrollY;
    _scrollY = newScrollY;
    return rows;
  }

  void TextArea::changeScrollHeight(int delta) {
    auto oldScrollHeight = _scrollHeight;

    const int newHeight = _scrollHeight + delta;
    _scrollHeight = newHeight < 0 ? 0 : newHeight;
    bool adjustScrollY = false;
    if (_scrollHeight != oldScrollHeight) {
      const int visibleScrollHeight = _scrollHeight - _scrollY;
      if (visibleScrollHeight < height() && _scrollY != 0) {
        const int rows = height() - visibleScrollHeight;
        vscroll(rows, height());
        adjustScrollY = true;
      }
      _scrollerChanges.capture(true, adjustScrollY, _scrollY);
    }
  }

  int TextArea::getLineBaseY(int lineIndex) {
    int baseY = -1;
    for(int i = 0; i <= lineIndex && i < _lines.size(); ++i) {
      baseY += _lines[i]->lineHeight();
    }
    return baseY - _scrollY;
  }

  int TextArea::getLineBaseX(int charX, int lineIndex) {
    const auto line = _lines[lineIndex];
    int baseX = DEFAULT_SIDE_MARGIN;
    for(int i = 0; i < charX && i < line->size(); ++i) {
      baseX += line->characters(i).getChWidth();
    }
    return baseX;
  }

  TextArea::VirtualYInfo TextArea::getLineAtVirtualY(const int baseY, const int rows) {
    const int virtualY = baseY + rows;
    int lineIndex;
    int y;
    for (lineIndex = 0, y = 0; lineIndex < _lines.size(); ++lineIndex) {
      int ny = y + _lines[lineIndex]->lineHeight();
      if (ny > virtualY || lineIndex == (_lines.size() - 1)) {
        break;
      }
      y = ny;
    }
    VirtualYInfo info;
    info._lineIndex = lineIndex;
    info._lineTopY = y - baseY;
    info._lineBaseY = info._lineTopY + _lines[lineIndex]->lineHeight() - 1;
    return info;
  }

  void TextArea::renderLineTopDown(const VirtualYInfo& info) {
    for(int ty = info._lineTopY, li = info._lineIndex; ty < height() && li < _lines.size(); ++li) {
      auto& line = _lines[li];
      ty += line->lineHeight();
      renderLine(*line, 0, li, ty - 1);
    }
  }

  void TextArea::renderLineBottomUp(const VirtualYInfo& info) {
    for(int by = info._lineBaseY, li = info._lineIndex; by >= 0 && li >= 0; --li) {
      auto& line = _lines[li];
      renderLine(*line, 0, li, by);
      by -= line->lineHeight();
    }
  }

  void TextArea::renderLineRange(const Position& p1, const Position& p2) {
    int lineBaseY = getLineBaseY(p1.y());
    for (int y = p1.y(); y <= p2.y() && y < _lines.size(); ++y) {
      auto line = _lines[y];
      renderLine(*line, 0, y, lineBaseY);
      lineBaseY += line->lineHeight();
    }
  }

  void TextArea::vscroll(int rows, int scrollableHeight) {
    upan::mutex_guard g(_drawMutex);
    updateCursor(false);
    rows = -rows;
    const bool scrollDown = rows > 0;
    rows = abs(updateScrollY(rows));
    if (rows > 0) {
      if (rows < scrollableHeight) {
        if (scrollDown) {
          const int srcY = MAX_FONT_SIZE + rows;
          const int destY = MAX_FONT_SIZE;
          const int moveLength = scrollableHeight - rows;
          memmove(_textBuffer.buffer() + destY * width(),
                  _textBuffer.buffer() + srcY * width(),
                  moveLength * width() * _textBuffer.bytesPerPixel());
          renderLineTopDown(getLineAtVirtualY(_scrollY, moveLength - 1));
        } else {
          const int srcY = MAX_FONT_SIZE;
          const int destY = MAX_FONT_SIZE + rows;
          const int moveLength = scrollableHeight - rows;
          memmove(_textBuffer.buffer() + destY * width(),
                  _textBuffer.buffer() + srcY * width(),
                  moveLength * width() * _textBuffer.bytesPerPixel());
          renderLineBottomUp(getLineAtVirtualY(_scrollY, rows - 1));
        }
      } else {
        renderLineTopDown(getLineAtVirtualY(_scrollY, 0));
      }
      _cursorPos.set(_cursorPos.x(), _cursorPos.y() + (scrollDown ? -rows : rows));
      notifyChange(ChangeState::Content);
    }
    updateCursor(true);
  }

  void TextArea::moveCursor(bool isSelectionOn, int x, int y) {
    const auto& info = getLineAtVirtualY(_scrollY, y);
    const int charPosY = info._lineIndex;
    const int curPosY = info._lineBaseY;

    auto line = _lines[charPosY];
    int charPosX = 0;
    int curPosX = DEFAULT_SIDE_MARGIN;

    while(charPosX < line->characters().size()) {
      auto ch = line->characters(charPosX);
      int nposX = curPosX + ch.getChWidth();
      if (nposX > x) {
        break;
      }
      curPosX = nposX;
      ++charPosX;
    }

    const Position prevCharPos = _characterPos;
    updateCursorPosition(charPosX, charPosY, curPosX, curPosY);
    scrollToCursor();
    updateSelectedArea(isSelectionOn, true, prevCharPos);

    notifyChange(ChangeState::Content);
  }

  void TextArea::handleMouseEvent(upanui::UIObject &sender, const upanui::MouseEvent &event) {
    const auto &e = event.getData();
    if (e.anyButtonPressed() || e.anyButtonHeld()) {
      moveCursor(e.isShiftPressed() || e.anyButtonHeld(), event.viewX() - drawX(), event.viewY() - drawY());
    }
  }

  void TextArea::deleteSelectedArea() {
    if (!_selectedArea.isPresent()) {
      return;
    }

    const Position& p1 = _selectedArea.p1();
    const int baseY = getLineBaseY(p1.y());
    const int baseX = getLineBaseX(p1.x(), p1.y());

    updateCursorPosition(p1.x(), p1.y(), baseX, baseY);
    scrollToCursor();
    unselectArea();

    int charCount = 0;
    if (_selectedArea.p1().y() == _selectedArea.p2().y()) {
      charCount = _selectedArea.p2().x() - _selectedArea.p1().x();
    } else {
      const int deleteLineY = _selectedArea.p1().y() + 1;
      int lineCountToDelete = _selectedArea.p2().y() - deleteLineY;
      while(lineCountToDelete > 0) {
        lineremovech(deleteLineY);
        --lineCountToDelete;
      }
      auto line1 = _lines[_selectedArea.p1().y()];
      charCount += (line1->size() - _selectedArea.p1().x()) + (line1->wrapped() ? 0 : 1);
      charCount += _selectedArea.p2().x();
    }

    while(charCount > 0) {
      removech();
      --charCount;
    }
  }

  bool TextArea::isSelectKey(uint8_t ch) const {
    static const upan::set<KeyboardKeys> selectKeys({
      Keyboard_KEY_UP,
      Keyboard_KEY_DOWN,
      Keyboard_KEY_RIGHT,
      Keyboard_KEY_LEFT,
      Keyboard_KEY_HOME,
      Keyboard_KEY_END
    });

    return selectKeys.exists((KeyboardKeys)ch);
  }

  bool TextArea::isTextModifyKey(uint8_t ch) const {
    return !is_command_key(ch) || ch == Keyboard_ENTER || ch == Keyboard_DEL || ch == Keyboard_KEY_DEL || ch == Keyboard_BACKSPACE;
  }

  bool TextArea::isInsertableKey(uint16_t ch) const {
    return !is_command_key(ch) || ch == Keyboard_ENTER;
  }

  void TextArea::SelectedArea::setRange(const Position& pa, const Position& pb) {
    if (pa <= pb) {
      _p1 = pa;
      _p2 = pb;
    } else {
      _p1 = pb;
      _p2 = pa;
    }
    if (_p1 == _p2) {
      _present = false;
    }
  }

  bool TextArea::SelectedArea::inRange(int x, int y) const {
    if (y == _p1.y() && x < _p1.x()) {
      return false;
    }

    if (y == _p2.y() && x >= _p2.x()) {
      return false;
    }

    if (y < _p1.y() || y > _p2.y()) {
      return false;
    }

    return true;
  }

  void TextArea::Line::insert(int pos, const Character &ch) {
    _characters.insert(pos, ch);
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
      _width += ch.getChWidth();
      _maxChHeight = upan::max(_maxChHeight, ch.getChHeight());
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