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
  TextArea::TextArea(int x, int y, int width, int height, int leftMargin,
                     HorizontalPlacementType horizontalPlacementType,
                     VerticalPlacementType verticalPlacementType)
  : RectangleCanvas(x, y, width, height, horizontalPlacementType, verticalPlacementType),
    _scrollY(0), _scrollHeight(0),
    _currentFontSize(Character::DEFAULT_FONT_SIZE), _currentFontType(usfn::PreloadedFonts::VGA16), _currentStyle(usfn::STYLE_REGULAR),
    _currentFGColor(DEFAULT_FG_COLOR), _currentBGColor(DEFAULT_BG_COLOR), _leftMargin(leftMargin),
    _maxLineCharWidth(width), _cursorBlinkThread(*this), _mouseHandler(nullptr), _lines(*this) {
  }

  TextArea::~TextArea() {
    _cursorBlinkThread.stop();
  }

  void TextArea::init() {
    _maxLineCharWidth = width() - leftMargin() * 2;
    backgroundColor(0xFFFFFF | GCoreFunctions::ALPHA_MASK);
    auto& curLine = _lines.add(0);
    _selectedArea.setPresent(false);
    _textBuffer.init(width(), height(), backgroundColorWithAlpha());
    //cursor y is within the character pixel block.
    //the character rendering should ensure that the character is drawn with certain bottom margin
    //for the cursor to be visible at the last pixel row of the character block.
    _cursorPos.set(_leftMargin, curLine.lineHeight() - 1);
    _characterPos.set(0, 0);
    changeScrollHeight(curLine.lineHeight());
    _cursorBlinkThread.start();

    UIObjectImpl::captureMouseEvents(true);
    _mouseHandler.reset(new TextAreaMouseHandler(*this));
    registerMouseEventHandler(*_mouseHandler);
  }

  void TextArea::leftMargin(int lm) {
    if (width() <= lm * 2) {
      throw upan::exception(XLOC, "TextArea should have a min width > %d", lm);
    }
    _leftMargin = lm;
  }

  void TextArea::scrollToY(int curPosY, int charPosY) {
    int rows;
    if (curPosY < 0) {
      rows =  -(curPosY - _lines.get(charPosY).lineHeight() + 1);
    } else if (curPosY >= height()) {
      rows =  -(curPosY - height() + 1);
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

    if (_characterPos.x() > _lines.get(_characterPos.y()).size()) {
      throw upan::exception(XLOC, "invalid cursor column:%u @ line:%u", _characterPos.x(), _characterPos.y());
    }
  }

  void TextArea::enter() {
    auto& curLine = _lines.get(_characterPos.y());

    if (_characterPos.x() == 0 && _characterPos.y() > 0) {
      auto& prevLine = _lines.get(_characterPos.y() - 1);
      if (prevLine.wrapped()) {
        prevLine.wrapped(false);
        return;
      }
    }

    auto& newLine = _lines.add(_characterPos.y() + 1);

    for(int i = _characterPos.x(), j = 0; i < curLine.size(); ++i, ++j) {
      newLine.insert(j, curLine.characters()[i]);
    }

    newLine.wrapped(curLine.wrapped());
    curLine.remove(_characterPos.x(), curLine.characters().size());
    curLine.wrapped(false);
    curLine.render(_characterPos.x(), _characterPos.y(), _cursorPos.y());

    auto newCursorY = _cursorPos.y() + newLine.lineHeight();
    if (newCursorY < height()) {
      _textBuffer.move(newCursorY + 1, _cursorPos.y() + 1, height() - newCursorY - 1);
      newLine.render(0, _characterPos.y() + 1, newCursorY);
    }

    changeScrollHeight(newLine.lineHeight());
    bool unwrapNextLine = _characterPos.x() > 0 && newLine.wrapped();
    inlinemovehome();
    movedown();
    if (unwrapNextLine) {
      inlinemoveend();
      removech();
      inlinemovehome();
    }
  }

  void TextArea::selectAll() {
    _selectedArea.setPresent(false);

    if (_lines.empty()) {
      return;
    }

    if (_lines.size() == 1 && _lines.get(0).size() == 0) {
      return;
    }

    UIPosition p1, p2;
    p1.set(0, 0);
    p2.set(_lines.get(_lines.size() - 1).size(), _lines.size() - 1);
    _selectedArea.setPresent(true);
    _selectedArea.setPivot(p1);
    _selectedArea.setRange(p1, p2);

    _lines.renderLineRange(p1, p2, _scrollY);
  }

  void TextArea::cutSelection() {
    copySelection();
    editSelection();
  }

  void TextArea::copySelection() {
    _copyBuffer.clear();

    if (!_selectedArea.isPresent()) {
      return;
    }

    if (_selectedArea.p1().y() == _selectedArea.p2().y()) {
      auto& line = _lines.get(_selectedArea.p1().y());

      for(int x = _selectedArea.p1().x(); x < _selectedArea.p2().x() && x < line.size(); ++x) {
        _copyBuffer.push_back(line.characters(x));
      }
    } else {
      const Character newLineCharacter(Keyboard_ENTER, _currentFontSize, _currentFontType, _currentStyle, _currentFGColor, _currentBGColor);
      {
        auto& line1 = _lines.get(_selectedArea.p1().y());
        for (int x = _selectedArea.p1().x(); x < line1.size(); ++x) {
          _copyBuffer.push_back(line1.characters(x));
        }
        if (!line1.wrapped()) {
          _copyBuffer.push_back(newLineCharacter);
        }
      }

      {
        for (int y = _selectedArea.p1().y() + 1; y < _selectedArea.p2().y(); ++y) {
          auto& line = _lines.get(y);
          for (const auto &ch : line.characters()) {
            _copyBuffer.push_back(ch);
          }
          if (!line.wrapped()) {
            _copyBuffer.push_back(newLineCharacter);
          }
        }
      }

      {
        auto& line2 = _lines.get(_selectedArea.p2().y());
        for (int x = 0; x < _selectedArea.p2().x() && x < line2.size(); ++x) {
          _copyBuffer.push_back(line2.characters(x));
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

    editSelection();
    insert(createCharacter(ch));
  }

  void TextArea::insert(const Character& ch) {
    if (isNewLine(ch.getCh())) {
      enter();
      return;
    }

    doInsert(ch);
  }

  void TextArea::doInsert(const Character& ch) {
    auto& line = _lines.get(_characterPos.y());
    auto origLineCount = _lines.size();

    Characters characters;
    characters.push_back(ch);
    insert(line, _characterPos.x(), _characterPos.y(), characters);

    auto hasNewLine = _lines.size() != origLineCount;

    if (hasNewLine) {
      auto srcCursorY = _cursorPos.y();
      auto destCursorY = _cursorPos.y();
      for (int i = _characterPos.y() + 1; i < _lines.size(); ++i) {
        auto& l = _lines.get(i);
        if (l.wrapped()) {
          srcCursorY += l.lineHeight();
        } else {
          destCursorY = srcCursorY + l.lineHeight();
          break;
        }
      }

      if (destCursorY < height()) {
        _textBuffer.move(destCursorY + 1, srcCursorY + 1, height() - destCursorY - 1);
      }
    }

    auto cursorY = _cursorPos.y();
    for(int i = _characterPos.y(); i < _lines.size(); ++i) {
      auto& l = _lines.get(i);
      l.render(0, i, cursorY);
      if (!l.wrapped()) { break; }
      if (cursorY >= height()) { break; }
      cursorY += l.lineHeight();
    }

    if (line.size() == _characterPos.x()) {
      //This means the new character inserted has moved to the next line
      updateCursorPosition(1, _characterPos.y(), ch.getChWidth() + _leftMargin, _cursorPos.y());
      movedown();
    } else {
      updateCursorPosition(_characterPos.x() + 1, _characterPos.y(), _cursorPos.x() + ch.getChWidth(), _cursorPos.y());
    }
  }

  Character TextArea::createCharacter(uint16_t ch) const {
    return { ch, _currentFontSize, _currentFontType, _currentStyle, _currentFGColor, _currentBGColor };
  }

  void TextArea::insert(TextLine& line, int lineX, int lineY, const Characters& characters) {
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
        auto& nextLine = _lines.get(lineY);
        insert(nextLine, 0, lineY, wrapCharacters);
      } else {
        line.wrapped(true);
        auto& newLine = _lines.add(lineY);
        changeScrollHeight(newLine.lineHeight());
        insert(newLine, 0, lineY, wrapCharacters);
      }
    }
  }

  void TextArea::removech() {
    auto& line = _lines.get(_characterPos.y());
    if (!line.wrapped() && _characterPos.x() == line.characters().size() && _characterPos.x() > 0) {
      line.wrapped(true);
    }

    if (line.wrapped()) {
      const int deletedLine = _lines.wrapremovech(_characterPos.x(), _characterPos.y(), _maxLineCharWidth);
      int baseY = _cursorPos.y() - line.lineHeight();
      for (int i = _characterPos.y(); i < _lines.size(); ++i) {
        auto& l = _lines.get(i);
        if (baseY >= ((int)height() - 1)) {
          break;
        }
        baseY += l.lineHeight();
        l.render(0, i, baseY);
        if (!l.wrapped()) {
          break;
        }
      }

      if (deletedLine > 0) {
        changeScrollHeight(_lines.removech(deletedLine, _characterPos.y(), _scrollY));
      }
    } else {
      if (_characterPos.x() == line.characters().size()) {
        if (_characterPos.x() == 0) {
          changeScrollHeight(_lines.removech(_characterPos.y(), _characterPos.y(), _scrollY));
        }
      } else {
        line.remove(_characterPos.x(), _characterPos.x() + 1);
        line.render(_characterPos.x(), _characterPos.y(), _cursorPos.y());
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
    auto& curLine = _lines.get(_characterPos.y());
    auto& prevLine = _lines.get(_characterPos.y() - 1);

    const auto& characters = prevLine.characters();
    auto cursorY = _cursorPos.y() - curLine.lineHeight();
    int cursorX = _leftMargin;
    int charX = 0;
    for (; charX < characters.size(); ++charX) {
      auto ch = characters[charX];
      if ((cursorX + ch.getChWidth()) > _cursorPos.x()) {
        break;
      }
      cursorX += ch.getChWidth();
    }

    if (cursorY >= (prevLine.lineHeight() - 1)) {
      updateCursorPosition(charX, _characterPos.y() - 1, cursorX, cursorY);
    } else {
      const int destCursorY = prevLine.lineHeight() - 1;

      updateCursor(false);
      _characterPos.set(charX, _characterPos.y() - 1);

      _textBuffer.move(destCursorY + 1, cursorY + 1, height() - destCursorY - 1);
      prevLine.render(0, _characterPos.y(), destCursorY);

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
    auto& nextLine = _lines.get(_characterPos.y() + 1);
    const auto& characters = nextLine.characters();
    int cursorX = _leftMargin;
    int charX = 0;
    for (; charX < characters.size(); ++charX) {
      auto ch = characters[charX];
      if ((cursorX + ch.getChWidth()) > _cursorPos.x()) {
        break;
      }
      cursorX += ch.getChWidth();
    }

    int overflowY = _cursorPos.y() + nextLine.lineHeight() - height();
    if (overflowY < 0) {
      updateCursorPosition(charX, _characterPos.y() + 1, cursorX, _cursorPos.y() + nextLine.lineHeight());
    } else {
      updateCursor(false);
      _characterPos.set(charX, _characterPos.y() + 1);

      _textBuffer.move(0, overflowY + 1, _cursorPos.y() - overflowY);
      nextLine.render(0, _characterPos.y(), height() - 1);

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
      inlinemoveend();
      return;
    }

    auto& curLine = _lines.get(_characterPos.y());
    updateCursorPosition(_characterPos.x() - 1, _characterPos.y(),
                         _cursorPos.x() - curLine.characters()[_characterPos.x() - 1].getChWidth(), _cursorPos.y());
  }

  void TextArea::moveright() {
    auto& curLine = _lines.get(_characterPos.y());
    if (_characterPos.x() == curLine.size()) {
      if ((_characterPos.y()+1) == _lines.size()) {
        return;
      }
      inlinemovehome();
      movedown();
      return;
    }

    updateCursorPosition(_characterPos.x() + 1, _characterPos.y(),
                         _cursorPos.x() + curLine.characters()[_characterPos.x()].getChWidth(), _cursorPos.y());
  }

  void TextArea::movehome() {
    inlinemovehome();
  }

  void TextArea::inlinemovehome() {
    if (_characterPos.x() == 0) {
      return;
    }

    updateCursorPosition(0, _characterPos.y(), _leftMargin, _cursorPos.y());
  }

  void TextArea::moveend() {
    inlinemoveend();
  }

  void TextArea::inlinemoveend() {
    auto& curLine = _lines.get(_characterPos.y());
    if (_characterPos.x() == curLine.size()) {
      return;
    }

    auto cursorX = _cursorPos.x();
    auto charX = _characterPos.x();
    while(charX < curLine.size()) {
      cursorX += curLine.characters()[charX].getChWidth();
      ++charX;
    }

    updateCursorPosition(charX, _characterPos.y(), cursorX, _cursorPos.y());
  }

  void TextArea::pageup() {
    const int curLineTopY = (_cursorPos.y() + 1 - _lines.get(_characterPos.y()).lineHeight());
    const int rows = height() - curLineTopY;

    vscroll(rows, height());

    const auto& info = _lines.getLineInfo(_scrollY, 0);
    updateCursorPosition(0, info._lineIndex, _leftMargin, info._lineBaseY);

    _scrollerChanges.capture(false, true, _scrollY);
  }

  void TextArea::pagedown() {
    const int curLineTopY = (_cursorPos.y() + 1 - _lines.get(_characterPos.y()).lineHeight());
    const int rows = height() + curLineTopY;
    const bool lastPage = (_scrollHeight - curLineTopY - _scrollY) <= height();

    vscroll(-rows, height());

    const auto& info = _lines.getLineInfo(_scrollY, lastPage ? height() : 0);
    updateCursorPosition(0, info._lineIndex, _leftMargin, info._lineBaseY);
    inlinemoveend();

    _scrollerChanges.capture(false, true, _scrollY);
  }

  uint32_t TextArea::getChBgColor(int cx, int cy, const Character& ch) const {
    const static uint32_t selectedAreaBGColor = 0x81B3F0 | GCoreFunctions::ALPHA_MASK;
    return _selectedArea.isPresent() && _selectedArea.inRange(cx, cy) ? selectedAreaBGColor : (ch.getBgColor() | (backgroundColorAlpha() << 24));
  }

  void TextArea::unselectArea() {
    //unselect the selected area
    _selectedArea.setPresent(false);
    _lines.renderLineRange(_selectedArea.p1(), _selectedArea.p2(), _scrollY);
  }

  void TextArea::updateSelectedArea(bool isSelectionOn, bool isSelectKey, const UIPosition& prevCharPos, const UIPosition& newCharPos) {
    if (!isSelectionOn) {
      if (_selectedArea.isPresent() && isSelectKey) {
        unselectArea();
      }
    } else {
      if (!isSelectKey) {
        return;
      }

      UIPosition rp1, rp2;
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

      _lines.renderLineRange(rp1, rp2, _scrollY);
    }
  }

  void TextArea::ScrollerChanges::capture(bool calibrate, bool adjustScrollY, int scrollY) {
    if (!_calibrate) _calibrate = calibrate;
    if (!_adjustScrollY) _adjustScrollY = adjustScrollY;
    _scrollY = scrollY;
  }

  void TextArea::ScrollerChanges::apply(VerticalScroller& verticalScroller) {
    if (_calibrate) {
      verticalScroller.calibrateScrollbar();
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

    const UIPosition prevCharPos = _characterPos;
    auto ch = event.getData().getRch();

    if (isSelectKey(ch) || isTextModifyKey(ch)) {
      scrollToY(_cursorPos.y(), _characterPos.y());
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

      case Keyboard_KEY_PG_UP:
        pageup();
        break;

      case Keyboard_KEY_PG_DOWN:
        pagedown();
        break;

      case Keyboard_DEL:
      case Keyboard_KEY_DEL:
        if (_selectedArea.isPresent()) {
          editSelection();
        } else {
          removech();
        }
        break;

      case Keyboard_BACKSPACE:
        if (_selectedArea.isPresent()) {
          editSelection();
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

    updateSelectedArea(event.getData().isShiftPressed(), isSelectKey(ch), prevCharPos, _characterPos);
    notifyChange(ChangeState::Content);
  }

  void TextArea::doDraw() {
    upan::mutex_guard g(_drawMutex);
    _textBuffer.copy(drawBuffer());
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
    auto& line = _lines.get(_characterPos.y());
    if (_characterPos.x() < line.characters().size()) {
      const auto ch = line.characters(_characterPos.x());
      color = (showCursor ? ch.getFgColor() : getChBgColor(_characterPos.x(), _characterPos.y(), ch));
    }
    color |= GCoreFunctions::ALPHA_MASK;

    if (_cursorPos.y() >= 0 && _cursorPos.y() < height()) {
      _textBuffer.fill(_cursorPos.x() + 1, _cursorPos.y() - 1, _currentFontSize / 2 - 1, 1, color);
    }
  }

  int TextArea::updateScrollY(int sy) {
    int newScrollY = _scrollY;
    if (sy < 0) {
      newScrollY = _scrollY + sy;
      if (newScrollY < 0) {
        newScrollY = 0;
      }
    } else {
      const int availableRows = _scrollHeight - (_scrollY + height());
      if (availableRows > 0) {
        newScrollY = _scrollY + upan::min(availableRows, sy);
      }
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

  void TextArea::vscroll(int rows, int scrollableHeight) {
    upan::mutex_guard g(_drawMutex);
    updateCursor(false);
    rows = -rows;
    const bool scrollDown = rows > 0;
    rows = abs(updateScrollY(rows));
    if (rows > 0) {
      if (rows < scrollableHeight) {
        if (scrollDown) {
          const int moveLength = scrollableHeight - rows;
          _textBuffer.move(0, rows, moveLength);
          _lines.renderLineTopDown(_scrollY, moveLength - 1, height());
        } else {
          const int moveLength = scrollableHeight - rows;
          _textBuffer.move(rows, 0, moveLength);
          _lines.renderLineBottomUp(_scrollY, rows - 1);
        }
      } else {
        _lines.renderLineTopDown(_scrollY, 0, height());
      }
      _cursorPos.set(_cursorPos.x(), _cursorPos.y() + (scrollDown ? -rows : rows));
      notifyChange(ChangeState::Content);
    }
    updateCursor(true);
  }

  TextLines::LineCursorInfo TextArea::getLineCursorInfo(int x, int y) {
    return _lines.getLineCursorPos(x, y, _scrollY, _leftMargin);
  }

  void TextArea::moveCursor(bool shiftPressed, bool mouseHeld, int x, int y) {
    upan::mutex_guard g(_drawMutex);

    const auto& info = getLineCursorInfo(x, y);

    const UIPosition prevCharPos = _characterPos;
    updateCursorPosition(info._charPos.x(), info._charPos.y(), info._curPos.x(), info._curPos.y());
    scrollToY(_cursorPos.y(), _characterPos.y());
    updateSelectedArea(shiftPressed || mouseHeld, true, prevCharPos, _characterPos);

    notifyChange(ChangeState::Content);
  }

  void TextArea::handleMouseEvent(upanui::UIObject &sender, const upanui::MouseEvent &event) {
    const auto &e = event.getData();
    if (e.anyButtonPressed() || e.anyButtonHeld()) {
      moveCursor(e.isShiftPressed(), e.anyButtonHeld(), event.viewX() - drawX(), event.viewY() - drawY());
      getVerticalScroller().ifPresent([this](VerticalScroller& verticalScroller) { _scrollerChanges.apply(verticalScroller); });
    }
  }

  void TextArea::editSelection() {
    if (!_selectedArea.isPresent()) {
      return;
    }

    const UIPosition& p1 = _selectedArea.p1();
    const int baseY = _lines.getLineBaseY(p1.y(), _scrollY);
    const int baseX = _lines.getLineBaseX(p1.x(), p1.y(), _leftMargin);

    updateCursorPosition(p1.x(), p1.y(), baseX, baseY);
    scrollToY(_cursorPos.y(), _characterPos.y());
    unselectArea();

    int charCount = 0;
    if (_selectedArea.p1().y() == _selectedArea.p2().y()) {
      charCount = _selectedArea.p2().x() - _selectedArea.p1().x();
    } else {
      const int deleteLineY = _selectedArea.p1().y() + 1;
      int lineCountToDelete = _selectedArea.p2().y() - deleteLineY;
      while(lineCountToDelete > 0) {
        changeScrollHeight(_lines.removech(deleteLineY, _characterPos.y(), _scrollY));
        --lineCountToDelete;
      }
      auto& line1 = _lines.get(_selectedArea.p1().y());
      charCount += (line1.size() - _selectedArea.p1().x()) + (line1.wrapped() ? 0 : 1);
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
      Keyboard_KEY_END,
      Keyboard_KEY_PG_UP,
      Keyboard_KEY_PG_DOWN
    });

    return selectKeys.exists((KeyboardKeys)ch);
  }

  bool TextArea::isTextModifyKey(uint8_t ch) const {
    return !is_command_key(ch)
    || isNewLine(ch)
    || ch == Keyboard_DEL
    || ch == Keyboard_KEY_DEL
    || ch == Keyboard_BACKSPACE
    || ch == Keyboard_CTRL_V
    || ch == Keyboard_CTRL_X;
  }

  bool TextArea::isInsertableKey(uint16_t ch) const {
    return !is_command_key(ch) || isNewLine(ch);
  }

  bool TextArea::isNewLine(uint16_t ch) const {
    return ch == Keyboard_ENTER || ch == Keyboard_CTRL_J;
  }

  void TextArea::SelectedArea::setRange(const UIPosition& pa, const UIPosition& pb) {
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

  TextArea::CursorBlink::CursorBlink(TextArea& textArea) : upan::timer_thread(500), _textArea(textArea), _showCursorToggle(false) {
  }

  void TextArea::CursorBlink::on_timer_trigger() {
    _textArea.updateCursor(_showCursorToggle);
    _textArea.notifyChange(ChangeState::Content);
    _showCursorToggle = !_showCursorToggle;
  }
}