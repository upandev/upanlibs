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

#include <TerminalConsole.h>
#include <ColorPalettes.h>
#include <ConsoleBuffer.h>
#include <BaseFrame.h>
#include <GraphicsContext.h>
#include <fs.h>
#include <GCoreFunctions.h>

namespace upanui {
  TerminalConsole* TerminalConsole::_instance = nullptr;

  TerminalConsole::TerminalConsole(int maxRows, int maxColumns)
    : UIElement(0, 0, GraphicsContext::Instance().frame().frameBuffer().width(),
                GraphicsContext::Instance().frame().frameBuffer().height(),
                HorizontalPlacementType::ABSOLUTE, VerticalPlacementType::ABSOLUTE),
      _cursorPos(0),
      _charStyle(CharStyle::WHITE_ON_BLACK()),
      _consoleBuffer(*this, maxRows, maxColumns),
      _cursorBlinkThread(*this),
      _readerThread(*this),
      _usfnContext(nullptr) {
    try {
      gc().frame().fillRect(0, 0, width(), height(), GCoreFunctions::ALPHA_MASK);
      UIElement::drawBuffer().initLocal(gc().frame().frameBuffer());
      init_term_console();
      _usfnContext.reset(new upanui::usfn::Context());
      _usfnContext->Load(usfn::Context::GetPreloadedFont(usfn::PreloadedFonts::VGA16));
      _usfnContext->Select(usfn::FAMILY_ANY, nullptr, usfn::STYLE_REGULAR, 16);
      _textWriter.setFontContext(_usfnContext.get());
    } catch(upan::exception& e) {
      printf("\n Failed to load USFN font: %s", e.ErrorMsg().c_str());
    }
    _cursorBlinkThread.start();
    _readerThread.start();
    UIObjectImpl::setKeyboardFocusable(true);
  }

  TerminalConsole& TerminalConsole::Create(int maxRows, int maxColumns) {
    if (_instance != nullptr) {
      throw upan::exception(XLOC, "TerminalConsole is already created!");
    }
    //create on heap because you can't pass objects on stack as params to threads
    _instance = new TerminalConsole(maxRows, maxColumns);
    return *_instance;
  }

  TerminalConsole& TerminalConsole::Create() {
    auto& frame = GraphicsContext::Instance().frame();
    return Create(frame.frameBuffer().height() / 16, frame.frameBuffer().width() / 8);
  }

  TerminalConsole& TerminalConsole::Instance() {
    if (_instance == nullptr) {
      throw upan::exception(XLOC, "TerminalConsole is not created yet!");
    }
    return *_instance;
  }

  TerminalConsole::~TerminalConsole() noexcept {
    _readerThread.stop();
    _cursorBlinkThread.stop();
  }

  void TerminalConsole::setFontContext(upanui::usfn::Context* context) {
    _textWriter.setFontContext(context);
  }

  void TerminalConsole::puts(const char* msg, const upanui::CharStyle& style) {
    _consoleBuffer.message(msg, style);
  }

  void TerminalConsole::puts(const char* msg) {
    puts(msg, _charStyle);
  }

  void TerminalConsole::putc(const char ch, const upanui::CharStyle& style) {
    _consoleBuffer.character(ch, style);
  }

  void TerminalConsole::putc(const char ch) {
    putc(ch, _charStyle);
  }

  void TerminalConsole::moveCursor(int pos) {
    _consoleBuffer.moveCursor(pos);
  }

  int TerminalConsole::getCurPos() const {
    return _consoleBuffer.getCurPos();
  }

  void TerminalConsole::clearLine(int pos) {
    _consoleBuffer.clearLine(pos);
  }

  void TerminalConsole::clearScreen() {
    _consoleBuffer.clear();
  }

  void TerminalConsole::putChar(int iPos, byte ch, const upanui::CharStyle& style) {
    const int curPos = iPos / upanui::ConsoleBuffer::NO_BYTES_PER_CHARACTER;
    const unsigned x = (curPos % _consoleBuffer.maxColumns());
    const unsigned y = (curPos / _consoleBuffer.maxColumns());

    _textWriter.drawChar(*this, ch, x, y,
                         ColorPalettes::CP16::Get(style.getFGColor()),
                         ColorPalettes::CP16::Get(style.getBGColor() >> 4));
  }

  void TerminalConsole::scrollDown() {
    _textWriter.scrollDown(*this);
  }

  void TerminalConsole::gotoCursor() {
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

  void TerminalConsole::putCursor(bool show) {
    if (_cursorPos >= _consoleBuffer.maxRows() * _consoleBuffer.maxColumns()) {
      return;
    }

    const upanui::CharStyle style = _consoleBuffer.getChar(_cursorPos * upanui::ConsoleBuffer::NO_BYTES_PER_CHARACTER + 1);
    const auto color = show ? ColorPalettes::CP16::Get(style.getFGColor()) : ColorPalettes::CP16::Get(style.getBGColor() >> 4);
    const auto x = (_cursorPos % _consoleBuffer.maxColumns());
    const auto y = (_cursorPos / _consoleBuffer.maxColumns());

    _textWriter.drawCursor(*this, x, y, color);
  }

  void TerminalConsole::draw() {
    gc().frame().touch();
  }

  TerminalConsole::Reader::Reader(TerminalConsole &console) : _console(console) {
  }

  void TerminalConsole::Reader::run() {
    char buf[1024];
    while(is_active()) {
      auto n = read(STDOUT_FD, buf, 1023);
      if (n > 0) {
        buf[n] = '\0';
        _console._consoleBuffer.message(buf, _console._charStyle);
      }
    }
  }

  TerminalConsole::CursorBlink::CursorBlink(TerminalConsole& console) : upan::timer_thread(500), _console(console), _showCursorToggle(false) {
  }

  void TerminalConsole::CursorBlink::on_timer_trigger() {
    upan::mutex_guard g(_console._cursorMutex);
    _console.putCursor(_showCursorToggle);
    _showCursorToggle = !_showCursorToggle;
  }
}