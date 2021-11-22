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
  class ConsoleCanvas : public Canvas, public IConsole {
  protected:
    ~ConsoleCanvas();
  public:
    ConsoleCanvas(uint32_t maxRows, uint32_t maxColumns);
    ConsoleCanvas();

    uint32_t maxRows() const;
    uint32_t maxColumns() const;

    void rawputc(byte ch, const CharStyle& style, bool updateCursorOnScreen);
    void rawputa(const MChar* src, uint32_t rows, uint32_t columns, int curPos);
    void puts(const char* msg, const upanui::CharStyle& style);
    void puts(const char* msg);
    void putc(const char ch, const upanui::CharStyle& style);
    void putc(const char ch);
    void moveCursor(int pos);
    int getCurPos() const;
    void setCurPos(int pos);
    void clearLine(int pos);
    void clearScreen();

    void setFontContext(upanui::usfn::Context* context);

  private:
    void gotoCursor() override;
    void putChar(int iPos, byte ch, const upanui::CharStyle& attr) override;
    void scrollDown() override;
    void putCursor(bool show);

    class CursorBlink : public upan::timer_thread {
    public:
      explicit CursorBlink(ConsoleCanvas& console);
      void on_timer_trigger() override;
      ConsoleCanvas& _console;
      bool _showCursorToggle;
    };

  private:
    BaseFrame& _frame;
    int _cursorPos;
    TextWriter _textWriter;
    CharStyle _charStyle;
    ConsoleBuffer _consoleBuffer;
    upan::mutex _cursorMutex;
    CursorBlink _cursorBlinkThread;
    upan::uniq_ptr<upanui::usfn::Context> _usfnContext;
  };
}