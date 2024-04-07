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

#include <stdlib.h>
#include <cdisplay.h>
#include <CharStyle.h>

namespace upanui {
  class IConsole;

  class ConsoleBuffer {
  private:
    ConsoleBuffer(IConsole& console, byte* buffer, int rows, int columns, bool selfManagedBuffer);
  public:
    static constexpr int START_CURSOR_POS = -1;
    static constexpr int NO_BYTES_PER_CHARACTER = 2;

    ConsoleBuffer(IConsole& console, byte* buffer, int rows, int columns);
    ConsoleBuffer(IConsole& console, int rows, int columns);
    ~ConsoleBuffer();

    int getBufferSize() const { return _bufSize; }
    int maxRows() const { return _maxRows; }
    int maxColumns() const { return _maxColumns; }

    int getCurPos() const { return _cursorPos; }
    void setCurPos(int curPos, bool updateCursorOnScreen);

    byte getChar(int pos) { return _buffer[pos]; }
    void message(const char* message, const CharStyle& attr);
    void nmessage(const char* message, int n, const CharStyle& attr);

    void moveCursor(int iOffSet);
    void clearLine(int iStartPos);
    void clear();
    void refresh();

    void rawCharacter(byte ch, const CharStyle& attr, bool updateCursorOnScreen);
    void rawCharacterArea(const MChar* src, int rows, int cols, int curPos);
    void character(char ch, const CharStyle& attr);

  private:
    bool putCharOnBuffer(int pos, byte val);
    void putChar(int iPos, byte ch, byte attr);
    void nextLine();


    void scrollDown();

    int getCurBytePos() const { return _cursorPos * NO_BYTES_PER_CHARACTER; }
    void _setCurPos(int curPos, bool updateCursorOnScreen);

  private:
    IConsole& _console;
    byte* _buffer;
    int _cursorPos;
    const int _bufSize;
    const int _maxRows;
    const int _maxColumns;
    const bool _selfManagedBuffer;
  };
}