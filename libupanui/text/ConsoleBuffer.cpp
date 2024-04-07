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

#include <ColorPalettes.h>
#include <cdisplay.h>
#include <string.h>
#include "ConsoleBuffer.h"
#include "IConsole.h"

namespace upanui {

//delegating constructor
ConsoleBuffer::ConsoleBuffer(IConsole& console, byte* buffer, int rows, int columns, bool selfManagedBuffer) :
  _console(console), _buffer(buffer), _cursorPos(0), _bufSize(rows * columns * ConsoleBuffer::NO_BYTES_PER_CHARACTER),
  _maxRows(rows), _maxColumns(columns), _selfManagedBuffer(selfManagedBuffer) {
  for(int i = 0; i < _bufSize; i += NO_BYTES_PER_CHARACTER) {
    _buffer[i] = ' ';
    _buffer[i + 1] = CharStyle::WHITE_ON_BLACK();
  }
}

ConsoleBuffer::ConsoleBuffer(IConsole& console, byte* buffer, int rows, int columns) :
  ConsoleBuffer(console, buffer, rows, columns, false) {
}

ConsoleBuffer::ConsoleBuffer(IConsole& console, int rows, int columns) :
    ConsoleBuffer(console, new byte[rows * columns * ConsoleBuffer::NO_BYTES_PER_CHARACTER], rows, columns, true) {
}

ConsoleBuffer::~ConsoleBuffer() {
  if (_selfManagedBuffer) {
    delete[] _buffer;
  }
}

void ConsoleBuffer::clear() {
  for(auto i = 0; i < _bufSize; i += NO_BYTES_PER_CHARACTER) {
    putChar(i, ' ', CharStyle::WHITE_ON_BLACK());
  }
  setCurPos(0, true);
}

void ConsoleBuffer::_setCurPos(int curPos, bool updateCursorOnScreen) {
  _cursorPos = curPos;
  if(updateCursorOnScreen) {
    _console.gotoCursor();
  }
}

void ConsoleBuffer::setCurPos(int curPos, bool updateCursorOnScreen) {
  if(curPos >= 0 && curPos < (_maxRows * _maxColumns)) {
    _setCurPos(curPos, updateCursorOnScreen);
  }
}

void ConsoleBuffer::moveCursor(int iOffSet) {
  setCurPos(getCurPos() + iOffSet, true);
}

void ConsoleBuffer::nextLine() {
  int curLine = (getCurPos() / _maxColumns) + 1;
  _setCurPos(curLine * _maxColumns, true);
  scrollDown();
}

void ConsoleBuffer::clearLine(int iStartPos) {
  if(iStartPos == START_CURSOR_POS)
    iStartPos = getCurPos();
  else if(!(iStartPos >= 0 && iStartPos < _maxColumns))
    iStartPos = getCurPos();

  for(int i = iStartPos * 2; (i % (_maxColumns * 2)) != 0; i += 2)
    putChar(i, ' ', CharStyle::WHITE_ON_BLACK());
  setCurPos(iStartPos, true);
}

void ConsoleBuffer::rawCharacterArea(const MChar* src, int rows, int cols, int curPos) {
  setCurPos(curPos, false);
  int i = 0;
  for(auto r = 0; r < rows; ++r) {
    for(auto c = 0; c < cols; ++c) {
      rawCharacter(src[i]._ch, src[i]._attr, false);
      ++i;
    }
    curPos += maxColumns();
    setCurPos(curPos, false);
  }
}

void ConsoleBuffer::rawCharacter(byte ch, const CharStyle& attr, bool updateCursorOnScreen) {
  if (ch == NO_CHAR) ch = ' ';
  putChar(getCurBytePos(), ch, attr);
  _setCurPos(getCurPos() + 1, updateCursorOnScreen);
}

void ConsoleBuffer::character(char ch, const CharStyle& attr) {
  if(ch == '\n') {
    nextLine();
  }
  else if(ch == '\t') {
    int i;
    for(i = 0; i < 4; i++)
      character(' ', attr);
  }
  else {
    putChar(getCurBytePos(), ch, attr);
    _setCurPos(getCurPos() + 1, true);
    scrollDown();
  }
}

void ConsoleBuffer::message(const char* message, const CharStyle& attr) {
  for(int i = 0; message[i] != '\0'; i++)
    character(message[i], attr);
}

void ConsoleBuffer::nmessage(const char* message, int n, const CharStyle& attr) {
  for(int i = 0; i < n; i++)
    character(message[i], attr);
}

bool ConsoleBuffer::putCharOnBuffer(int pos, byte val) {
  bool changed = _buffer[pos] != val;
  if (changed) {
    _buffer[pos] = val;
  }
  return changed;
}

void ConsoleBuffer::putChar(int iPos, byte ch, byte attr) {
  const bool charChanged = putCharOnBuffer(iPos, ch);
  const bool attrChanged = putCharOnBuffer(iPos + 1, attr);
  const bool changed = charChanged || attrChanged;
  if(changed)
    _console.putChar(iPos, ch, attr);
}

void ConsoleBuffer::scrollDown() {
  if(getCurPos() < (_maxRows * _maxColumns))
    return;

  const unsigned NO_OF_DISPLAY_BYTES = (_maxRows - 1) * _maxColumns * NO_BYTES_PER_CHARACTER;
  const unsigned OFFSET = _maxColumns * NO_BYTES_PER_CHARACTER;

  memcpy(_buffer, _buffer + OFFSET, NO_OF_DISPLAY_BYTES);
  for(unsigned i = NO_OF_DISPLAY_BYTES; i < NO_OF_DISPLAY_BYTES + _maxColumns * NO_BYTES_PER_CHARACTER; i += NO_BYTES_PER_CHARACTER) {
    _buffer[i] = ' ';
    _buffer[i + 1] = CharStyle::WHITE_ON_BLACK();
  }

  _console.scrollDown();

  _setCurPos(NO_OF_DISPLAY_BYTES / 2, true);
}

void ConsoleBuffer::refresh() {
  const unsigned noOfDisplayBytes = getBufferSize();

  for(unsigned i = 0; i < noOfDisplayBytes; i += NO_BYTES_PER_CHARACTER)
    _console.putChar(i, _buffer[i], _buffer[i+1]);

  _console.gotoCursor();
}

}