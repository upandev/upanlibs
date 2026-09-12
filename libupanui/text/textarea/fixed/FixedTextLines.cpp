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

#include <FixedTextLines.h>
#include <FixedTextArea.h>
#include <FixedTextLine.h>

namespace upanui {
  FixedTextLines::~FixedTextLines() {
    clear();
  }

  void FixedTextLines::clear() {
    for(auto i : _lines) {
      delete i;
    }
    _lines.clear();
  }

  void FixedTextLines::realignOnWidthIncrease() {
    for(int li = 0; li < _lines.size();) {
      auto line = _lines[li];
      if (line->wrapped()) {
        int availChars = _textArea._maxLineChars - line->size();
        auto nextLine = _lines[li + 1];
        int i;
        for (i = 0; i < availChars && i < nextLine->size(); ++i) {
          auto ch = nextLine->FixedCharacters(i);
          line->insert(line->size(), ch);
        }

        nextLine->remove(0, i);

        if (nextLine->size()) {
          ++li;
        } else {
          _lines.erase(li + 1, li + 2);
          if (!nextLine->wrapped()) {
            line->wrapped(false);
            ++li;
          }
          delete nextLine;
        }
      } else {
        ++li;
      }
    }
  }

  void FixedTextLines::realignOnWidthDecrease() {
    FixedCharacters wrapFixedCharacters;
    for (int li = 0; li < _lines.size(); ++li) {
      auto line = _lines[li];

      for(int i = 0; i < wrapFixedCharacters.size(); ++i) {
        line->insert(i, wrapFixedCharacters[i]);
      }

      wrapFixedCharacters.clear();
      if (line->size() > _textArea._maxLineChars) {
        for (int i = _textArea._maxLineChars; i < line->size(); ++i) {
          wrapFixedCharacters.push_back(line->FixedCharacters(i));
        }
        line->remove(_textArea._maxLineChars, line->size());
      }

      if (!wrapFixedCharacters.empty() && !line->wrapped()) {
        line->wrapped(true);
        add(li + 1);
      }
    }
  }

  FixedTextLine& FixedTextLines::add(int index) {
    auto line = new FixedTextLine(_textArea);
    _lines.insert(index, line);
    return *line;
  }

  FixedTextLine& FixedTextLines::get(int index) const {
    if (index >= size()) {
      throw upan::exception(XLOC, "invalid line index: %d", index);
    }
    return *_lines[index];
  }

  int FixedTextLines::wrapremovech(int x, int y, int maxLineChars) {
    int deletedLine = -1;
    auto& line = get(y);
    line.remove(x, x + 1);

    if (!line.wrapped()) {
      return deletedLine;
    }

    auto ny = y + 1;
    if (ny < _lines.size()) {
      auto& nextLine = *_lines[ny];
      int availChars = maxLineChars - line.size();
      bool deletedFromNextLine = false;
      while (nextLine.size() > 0 && availChars > 0) {
        auto ch = nextLine.FixedCharacters(0);
        line.insert(line.size(), ch);
        --availChars;
        deletedLine = wrapremovech(0, ny, maxLineChars);
        deletedFromNextLine = true;
      }

      //deleting when cursor is at the end of current line which is full - then we need to remove the first char from next line
      if (!deletedFromNextLine && nextLine.size() > 0 && x == line.size()) {
        deletedLine = wrapremovech(0, ny, maxLineChars);
      }
      if (nextLine.size() == 0) {
        line.wrapped(false);
        deletedLine = ny;
      }
    }
    return deletedLine;
  }

  int FixedTextLines::removeLine(const int y, const int fixedCharacterPosY, const int scrollBaseY) {
    const int baseY = getLineBaseY(y, scrollBaseY);
    auto& line = get(y);
    auto visibleBaseY = baseY - _textArea.lineHeight() + 1;
    auto insideCanvas = visibleBaseY < _textArea.height();
    const int deletedLineHeight = _textArea.lineHeight();

    if (y > 0) {
      auto& prevLine = get(y - 1);
      if (prevLine.wrapped()) {
        prevLine.wrapped(line.wrapped());
      }
    }

    if (y < (_lines.size() - 1) && insideCanvas) {
      int copyHeight = 0;
      int lastLineIndex = -1;
      auto lastLineBaseCursorY = baseY;

      for (int i = y + 1; i < _lines.size(); ++i) {
        const int lineHeight = _textArea.lineHeight();
        lastLineBaseCursorY += lineHeight;
        if (lastLineBaseCursorY >= (_textArea.height() - 1)) {
          lastLineIndex = i;
          break;
        }
        copyHeight += lineHeight;
      }

      _textArea.textBuffer().move(visibleBaseY, baseY + 1, copyHeight);

      const int destYOnCanvas = visibleBaseY + copyHeight;
      if (copyHeight > 0) {
        _textArea.textBuffer().clear(0, destYOnCanvas, _textArea.width(), _textArea.height() - destYOnCanvas);
      }

      for (int lastLineTopY = destYOnCanvas; lastLineTopY < _textArea.height() && lastLineIndex != -1 && lastLineIndex < _lines.size();) {
        auto lastLine = _lines[lastLineIndex];
        lastLine->render(0, lastLineIndex, lastLineTopY + _textArea.lineHeight() - 1);
        lastLineTopY += _textArea.lineHeight();
        ++lastLineIndex;
      }

      _lines.erase(y, y + 1);
      delete &line;
      return deletedLineHeight;
    } else if (fixedCharacterPosY < y) { //it's a no-op if deleting the line where FixedCharacter cursor is
      if (insideCanvas) {
        _textArea.textBuffer().clear(0, visibleBaseY, _textArea.width(), _textArea.lineHeight());
      }
      _lines.erase(y, y + 1);
      delete &line;
      return deletedLineHeight;
    } else {
      return 0;
    }
  }

  int FixedTextLines::getLineBaseY(int lineIndex, int scrollBaseY) {
    if (lineIndex >= _lines.size()) {
      lineIndex = _lines.size() - 1;
    }
    return (-1 + (lineIndex + 1) * _textArea.lineHeight() - scrollBaseY);
  }

  int FixedTextLines::getLineBaseX(int charX, int lineIndex, int leftMargin) {
    const auto& line = get(lineIndex);

    if (charX > line.size()) {
      charX = line.size();
    }

    return leftMargin + charX * _textArea.charWidth();
  }

  FixedTextLines::LineCursorInfo FixedTextLines::getLineCursorPos(const int x, const int y, const int baseY, const int leftMargin) {
    const auto& info = getLineInfo(baseY, y);
    const int charPosY = info._lineIndex;
    const int curPosY = info._lineBaseY;

    auto& line = get(charPosY);
    int charPosX = (x - leftMargin) / _textArea.charWidth();
    if (charPosX > line.size()) {
      charPosX = line.size();
    }
    int curPosX = leftMargin + charPosX * _textArea.charWidth();

    LineCursorInfo lineCursorInfo;
    lineCursorInfo._charPos.set(charPosX, charPosY);
    lineCursorInfo._curPos.set(curPosX, curPosY);
    return lineCursorInfo;
  }

  FixedTextLines::LineInfo FixedTextLines::getLineInfo(const int baseY, const int rows) {
    const int virtualY = baseY + rows;
    int lineCount = virtualY / _textArea.lineHeight();
    lineCount = lineCount > (_lines.size() - 1) ? (_lines.size() - 1) : lineCount;
    const int y = lineCount * _textArea.lineHeight();

    LineInfo info;
    info._lineIndex = lineCount;
    info._lineTopY = y - baseY;
    info._lineBaseY = info._lineTopY + _textArea.lineHeight() - 1;
    return info;
  }

  void FixedTextLines::renderLineTopDown(int baseY, int rows, int height) {
    const LineInfo info = getLineInfo(baseY, rows);
    for(int ty = info._lineTopY, li = info._lineIndex; ty < height && li < _lines.size(); ++li) {
      auto line = _lines[li];
      ty += _textArea.lineHeight();
      line->render(0, li, ty - 1);
    }
  }

  void FixedTextLines::renderLineBottomUp(int baseY, int rows) {
    const LineInfo info = getLineInfo(baseY, rows);
    for(int by = info._lineBaseY, li = info._lineIndex; by >= 0 && li >= 0; --li) {
      auto line = _lines[li];
      line->render(0, li, by);
      by -= _textArea.lineHeight();
    }
  }

  void FixedTextLines::renderLineRange(const UIPosition& p1, const UIPosition& p2, int baseY) {
    int lineBaseY = getLineBaseY(p1.y(), baseY);
    for (int y = p1.y(); y <= p2.y() && y < _lines.size(); ++y) {
      auto line = _lines[y];
      line->render(0, y, lineBaseY);
      lineBaseY += _textArea.lineHeight();
    }
  }

  int FixedTextLines::calculateCharCount(int x, int y) {
    int charCount = 0;
    for(int i = 0; i < _lines.size(); ++i) {
      const int lineSize = _lines[i]->size();
      if (i == y) {
        charCount += (x > lineSize ? lineSize : x);
        break;
      }
      charCount += _lines[i]->size();
    }
    return charCount;
  }

  int FixedTextLines::calculateHeight() {
    return _lines.size() * _textArea.lineHeight();
  }
}