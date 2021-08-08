/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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

#include <ConsoleCanvas.h>
#include <ColorPalettes.h>
#include <ConsoleBuffer.h>
#include <BaseFrame.h>
#include <fs.h>

namespace upanui {
  ConsoleCanvas::ConsoleCanvas(BaseFrame& frame, uint32_t maxRows, uint32_t maxColumns)
    : _frame(frame), _cursorPos(0),
      _charStyle(CharStyle::WHITE_ON_BLACK()),
      _consoleBuffer(*this, maxRows, maxColumns),
      _cursorBlinkThread(*this),
      _readerThread(*this) {
    _cursorBlinkThread.run();
    _readerThread.run();
  }

  ConsoleCanvas::ConsoleCanvas(BaseFrame& frame)
    : ConsoleCanvas(frame, frame.frameBuffer().height() / 16, frame.frameBuffer().width() / 8) {
  }

  ConsoleCanvas::~ConsoleCanvas() noexcept {
    _readerThread.stop();
    _cursorBlinkThread.stop();
  }

  uint32_t ConsoleCanvas::width() const {
    return _frame.frameBuffer().width();
  }

  uint32_t ConsoleCanvas::height() const {
    return _frame.frameBuffer().height();
  }

  const uint32_t* ConsoleCanvas::dataBuffer() const {
    return _frame.frameBuffer().buffer();
  }

  void ConsoleCanvas::setFontContext(upanui::usfn::Context* context) {
    _textWriter.setFontContext(context);
  }

  void ConsoleCanvas::putChar(int iPos, byte ch, const upanui::CharStyle& style) {
    const int curPos = iPos / upanui::ConsoleBuffer::NO_BYTES_PER_CHARACTER;
    const unsigned x = (curPos % _consoleBuffer.maxColumns());
    const unsigned y = (curPos / _consoleBuffer.maxColumns());

    _textWriter.drawChar(_frame, ch, x, y,
                         ColorPalettes::CP16::Get(style.getFGColor()),
                         ColorPalettes::CP16::Get(style.getBGColor() >> 4));
  }

  void ConsoleCanvas::scrollDown() {
    _textWriter.scrollDown(_frame);
  }

  void ConsoleCanvas::gotoCursor() {
    upan::mutex_guard g(_cursorMutex);
    int newCurPos = _consoleBuffer.getCurPos();
    if (newCurPos != _cursorPos) {
      //erase old cursor
      putCursor(false);
    }
    //draw new cursor
    _cursorPos = newCurPos;
    putCursor(true);
  }

  void ConsoleCanvas::putCursor(bool show) {
    if ((uint32_t)_cursorPos >= _consoleBuffer.maxRows() * _consoleBuffer.maxColumns()) {
      return;
    }

    const upanui::CharStyle style = _consoleBuffer.getChar(_cursorPos * upanui::ConsoleBuffer::NO_BYTES_PER_CHARACTER + 1);
    const auto color = show ? ColorPalettes::CP16::Get(style.getFGColor()) : ColorPalettes::CP16::Get(style.getBGColor() >> 4);
    const auto x = (_cursorPos % _consoleBuffer.maxColumns());
    const auto y = (_cursorPos / _consoleBuffer.maxColumns());

    _textWriter.drawCursor(_frame, x, y, color);
  }

  ConsoleCanvas::Reader::Reader(ConsoleCanvas &console) : upan::timer_thread(50), _console(console) {
  }

  void ConsoleCanvas::Reader::on_timer_trigger() {
    char buf[1024];
    auto n = read(STDOUT_FD, buf, 1024);
    if (n > 0) {
      buf[n] = '\0';
      _console._consoleBuffer.message(buf, _console._charStyle);
    }
  }

  ConsoleCanvas::CursorBlink::CursorBlink(ConsoleCanvas& console) : upan::timer_thread(500), _console(console), _showCursorToggle(false) {
  }

  void ConsoleCanvas::CursorBlink::on_timer_trigger() {
    upan::mutex_guard g(_console._cursorMutex);
    _console.putCursor(_showCursorToggle);
    _showCursorToggle = !_showCursorToggle;
  }
}