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

#include <vector.h>

namespace upanui {
  class TextArea;
  class TextLine;
  class UIPosition;

  class TextLines {
  public:
    TextLines(TextArea& textArea) : _textArea(textArea) {}
    ~TextLines();

    TextLines(const TextLines&) = delete;
    TextLines& operator=(const TextLines&) = delete;

    int size() const { return _lines.size(); }
    bool empty() const { return _lines.empty(); }

    TextLine& add(int index);
    TextLine& get(int index) const;
    int removech(int y, int characterPosY, int scrollBaseY);

    typedef struct {
      int _lineIndex;
      int _lineTopY;
      int _lineBaseY;
    } LineInfo;

    int getLineBaseY(int lineIndex, int scrollBaseY);
    int getLineBaseX(int charX, int lineIndex, int leftMargin);
    LineInfo getLineInfo(int baseY, int rows);
    void renderLineTopDown(int baseY, int rows, int height);
    void renderLineBottomUp(int baseY, int rows);
    void renderLineRange(const UIPosition& p1, const UIPosition& p2, int baseY);

  private:
    upan::vector<TextLine*> _lines;
    TextArea& _textArea;
  };
}