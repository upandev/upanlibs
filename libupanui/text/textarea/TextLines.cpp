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

#include <TextLines.h>
#include <TextArea.h>
#include <TextLine.h>

namespace upanui {
  TextLines::~TextLines() {
    for(auto i : _lines) {
      delete i;
    }
  }

  TextLine& TextLines::add(int index) {
    auto line = new TextLine(_textArea.currentFontSize(), _textArea);
    _lines.insert(index, line);
    return *line;
  }

  TextLine& TextLines::get(int index) const {
    if (index >= size()) {
      throw upan::exception(XLOC, "invalid line index: %d", index);
    }
    return *_lines[index];
  }

  int TextLines::wrapremovech(int x, int y, int maxLineChWidth) {
    int deletedLine = -1;
    auto& line = get(y);
    line.remove(x, x + 1);

    if (!line.wrapped()) {
      return deletedLine;
    }

    auto ny = y + 1;
    if (ny < _lines.size()) {
      auto& nextLine = *_lines[ny];
      int availWidth = maxLineChWidth - line.width();
      bool deletedFromNextLine = false;
      while (nextLine.size() > 0) {
        auto ch = nextLine.characters(0);
        if (ch.getChWidth() < availWidth) {
          line.insert(line.size(), ch);
          availWidth -= ch.getChWidth();
          wrapremovech(0, ny, maxLineChWidth);
          deletedFromNextLine = true;
        } else {
          break;
        }
      }
      //deleting when cursor is at the end of current line which is full - then we need to remove the first char from next line
      if (!deletedFromNextLine && nextLine.size() > 0 && x == line.size()) {
        wrapremovech(0, ny, maxLineChWidth);
      }
      if (nextLine.size() == 0) {
        line.wrapped(false);
        deletedLine = ny;
      }
    }
    return deletedLine;
  }

  int TextLines::removech(const int y, const int characterPosY, const int scrollBaseY) {
    const int baseY = getLineBaseY(y, scrollBaseY);
    auto& line = get(y);
    auto visibleBaseY = baseY - line.lineHeight() + 1;
    auto insideCanvas = visibleBaseY < _textArea.height();
    const int deletedLineHeight = line.lineHeight();

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
        const int lineHeight = get(i).lineHeight();
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
        lastLine->render(0, lastLineIndex, lastLineTopY + lastLine->lineHeight() - 1);
        lastLineTopY += lastLine->lineHeight();
        ++lastLineIndex;
      }

      _lines.erase(y, y + 1);
      delete &line;
    } else if (characterPosY < y) { //it's a no-op if deleting the line where character cursor is
      if (insideCanvas) {
        _textArea.textBuffer().clear(0, visibleBaseY, _textArea.width(), line.lineHeight());
      }
      _lines.erase(y, y + 1);
      delete &line;
    }
    return deletedLineHeight;
  }

  int TextLines::getLineBaseY(int lineIndex, int scrollBaseY) {
    int baseY = -1;
    for(int i = 0; i <= lineIndex && i < _lines.size(); ++i) {
      baseY += _lines[i]->lineHeight();
    }
    return baseY - scrollBaseY;
  }

  int TextLines::getLineBaseX(int charX, int lineIndex, int leftMargin) {
    const auto& line = get(lineIndex);
    int baseX = leftMargin;
    for(int i = 0; i < charX && i < line.size(); ++i) {
      baseX += line.characters(i).getChWidth();
    }
    return baseX;
  }

  TextLines::LineInfo TextLines::getLineInfo(const int baseY, const int rows) {
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
    LineInfo info;
    info._lineIndex = lineIndex;
    info._lineTopY = y - baseY;
    info._lineBaseY = info._lineTopY + _lines[lineIndex]->lineHeight() - 1;
    return info;
  }

  void TextLines::renderLineTopDown(int baseY, int rows, int height) {
    const LineInfo info = getLineInfo(baseY, rows);
    for(int ty = info._lineTopY, li = info._lineIndex; ty < height && li < _lines.size(); ++li) {
      auto line = _lines[li];
      ty += line->lineHeight();
      line->render(0, li, ty - 1);
    }
  }

  void TextLines::renderLineBottomUp(int baseY, int rows) {
    const LineInfo info = getLineInfo(baseY, rows);
    for(int by = info._lineBaseY, li = info._lineIndex; by >= 0 && li >= 0; --li) {
      auto line = _lines[li];
      line->render(0, li, by);
      by -= line->lineHeight();
    }
  }

  void TextLines::renderLineRange(const UIPosition& p1, const UIPosition& p2, int baseY) {
    int lineBaseY = getLineBaseY(p1.y(), baseY);
    for (int y = p1.y(); y <= p2.y() && y < _lines.size(); ++y) {
      auto line = _lines[y];
      line->render(0, y, lineBaseY);
      lineBaseY += line->lineHeight();
    }
  }
}