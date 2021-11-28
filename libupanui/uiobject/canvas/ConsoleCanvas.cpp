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

#include <ConsoleCanvas.h>
#include <ColorPalettes.h>
#include <ConsoleBuffer.h>
#include <BaseFrame.h>
#include <GraphicsContext.h>

namespace upanui {
  ConsoleCanvas::ConsoleCanvas(uint32_t maxRows, uint32_t maxColumns)
    : UIElement(0, 0, GraphicsContext::Instance().frame().frameBuffer().width(), GraphicsContext::Instance().frame().frameBuffer().height()),
      _cursorPos(0),
      _charStyle(CharStyle::WHITE_ON_BLACK()),
      _consoleBuffer(*this, maxRows, maxColumns),
      _cursorBlinkThread(*this),
      _usfnContext(nullptr) {
    try {
      _usfnContext.reset(new upanui::usfn::Context());
      _usfnContext->Load(upanui::usfn::Context::GetPreloadedFont(upanui::usfn::PreloadedFonts::VGA16));
      _usfnContext->Select(upanui::usfn::FAMILY_MONOSPACE, NULL, upanui::usfn::STYLE_REGULAR, 16);
      _textWriter.setFontContext(_usfnContext.get());
    } catch(upan::exception& e) {
      printf("\n Failed to load USFN font: %s", e.ErrorMsg().c_str());
    }
    _cursorBlinkThread.start();
  }

  ConsoleCanvas::ConsoleCanvas()
    : ConsoleCanvas(GraphicsContext::Instance().frame().frameBuffer().height() / 16,
                    GraphicsContext::Instance().frame().frameBuffer().width() / 8) {
  }

  ConsoleCanvas::~ConsoleCanvas() noexcept {
    _cursorBlinkThread.stop();
  }

  uint32_t ConsoleCanvas::maxRows() const {
    return _consoleBuffer.maxRows();
  }

  uint32_t ConsoleCanvas::maxColumns() const {
    return _consoleBuffer.maxColumns();
  }

  void ConsoleCanvas::setFontContext(upanui::usfn::Context* context) {
    _textWriter.setFontContext(context);
  }

  void ConsoleCanvas::rawputc(byte ch, const CharStyle& style, bool updateCursorOnScreen) {
    _consoleBuffer.rawCharacter(ch, style, updateCursorOnScreen);
  }

  void ConsoleCanvas::rawputa(const MChar* src, uint32_t rows, uint32_t columns, int curPos) {
    _consoleBuffer.rawCharacterArea(src, rows, columns, curPos);
  }

  void ConsoleCanvas::puts(const char* msg, const upanui::CharStyle& style) {
    _consoleBuffer.message(msg, style);
  }

  void ConsoleCanvas::puts(const char* msg) {
    puts(msg, _charStyle);
  }

  void ConsoleCanvas::putc(const char ch, const upanui::CharStyle& style) {
    _consoleBuffer.character(ch, style);
  }

  void ConsoleCanvas::putc(const char ch) {
    putc(ch, _charStyle);
  }

  void ConsoleCanvas::moveCursor(int pos) {
    _consoleBuffer.moveCursor(pos);
  }

  int ConsoleCanvas::getCurPos() const {
    return _consoleBuffer.getCurPos();
  }

  void ConsoleCanvas::setCurPos(int pos) {
    _consoleBuffer.setCurPos(pos, true);
  }

  void ConsoleCanvas::clearLine(int pos) {
    _consoleBuffer.clearLine(pos);
  }

  void ConsoleCanvas::clearScreen() {
    _consoleBuffer.clear();
  }

  void ConsoleCanvas::putChar(int iPos, byte ch, const upanui::CharStyle& style) {
    const int curPos = iPos / upanui::ConsoleBuffer::NO_BYTES_PER_CHARACTER;
    const unsigned x = (curPos % _consoleBuffer.maxColumns());
    const unsigned y = (curPos / _consoleBuffer.maxColumns());

    _textWriter.drawChar(gc().frame(), ch, x, y,
                         ColorPalettes::CP16::Get(style.getFGColor()),
                         ColorPalettes::CP16::Get(style.getBGColor() >> 4));
  }

  void ConsoleCanvas::scrollDown() {
    _textWriter.scrollDown(gc().frame());
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

    _textWriter.drawCursor(gc().frame(), x, y, color);
  }

  ConsoleCanvas::CursorBlink::CursorBlink(ConsoleCanvas& console) : upan::timer_thread(500), _console(console), _showCursorToggle(false) {
  }

  void ConsoleCanvas::CursorBlink::on_timer_trigger() {
    upan::mutex_guard g(_console._cursorMutex);
    _console.putCursor(_showCursorToggle);
    _showCursorToggle = !_showCursorToggle;
  }
}