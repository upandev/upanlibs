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

#include <Canvas.h>
#include <timer_thread.h>
#include <TextWriter.h>
#include <IConsole.h>
#include <ConsoleBuffer.h>
#include <uniq_ptr.h>

namespace upanui {
  class TerminalConsole : public UIElement, public IConsole {
  private:
    TerminalConsole(int maxRows, int maxColumns);
    static TerminalConsole* _instance;

  public:
    static TerminalConsole& Create(int maxRows, int maxColumns);
    static TerminalConsole& Create();
    static TerminalConsole& Instance();

    ~TerminalConsole();

    void puts(const char* msg, const upanui::CharStyle& style);
    void puts(const char* msg);
    void putc(const char ch, const upanui::CharStyle& style);
    void putc(const char ch);
    void moveCursor(int pos);
    int getCurPos() const;
    void clearLine(int pos);
    void clearScreen();

    void setFontContext(upanui::usfn::Context* context);
    bool isRectangularShape() override { return true; }

  private:
    void draw() override;
    void drawTopDown() override {}
    void drawToTop() override {}
    Layout& layout() override {
      throw upan::exception(XLOC, "layout not supported yet!");
    }

    void gotoCursor() override;
    void putChar(int iPos, byte ch, const upanui::CharStyle& attr) override;
    void scrollDown() override;
    void putCursor(bool show);

    class Reader : public upan::thread {
    public:
      explicit Reader(TerminalConsole& console);
      void run() override;
      TerminalConsole& _console;
    };

    class CursorBlink : public upan::timer_thread {
    public:
      explicit CursorBlink(TerminalConsole& console);
      void on_timer_trigger() override;
      TerminalConsole& _console;
      bool _showCursorToggle;
    };

  private:
    int _cursorPos;
    TextWriter _textWriter;
    CharStyle _charStyle;
    ConsoleBuffer _consoleBuffer;
    upan::mutex _cursorMutex;
    CursorBlink _cursorBlinkThread;
    Reader _readerThread;
    upan::uniq_ptr<upanui::usfn::Context> _usfnContext;
  };
}