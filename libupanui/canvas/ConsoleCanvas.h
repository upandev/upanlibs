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

#pragma once

#include <Canvas.h>
#include <FrameManager.h>
#include <timer_thread.h>
#include <libupanui/text/TextWriter.h>

namespace upanui {
  class ConsoleCanvas : public Canvas, public upan::timer_thread {
  public:
    ConsoleCanvas(FrameManager& parent, uint32_t maxRows, uint32_t maxColumns);
    void on_timer_trigger() override;
    //    void GotoCursor() override;
    //    void DoScrollDown() override;
    //    void PutCursor(int pos, bool show);
    void StartCursorBlink();
    //

  private:
    void DirectPutChar(int iPos, byte ch, byte attr);
    //    friend class Display;
    //    int _cursorPos;
    //    bool _cursorEnabled;
    //    Mutex _cursorMutex;
  private:
    FrameManager& _parent;
    uint32_t _maxRows;
    uint32_t _maxColumns;
    int _cursorPos;
    bool _cursorEnabled;
    upan::mutex _cursorMutex;
    TextWriter _textWriter;
  };
}