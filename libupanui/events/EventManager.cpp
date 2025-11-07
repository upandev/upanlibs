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

#include <EventManager.h>
#include <KeyboardEventHandler.h>
#include <MouseEventHandler.h>
#include <GraphicsContext.h>
#include <fs.h>
#include <mosstd.h>
#include <MouseData.h>

namespace upanui {
  EventManager::EventManager() {
  }

  void EventManager::startEventLoop() {
    startEventLoop(false);
  }

  void EventManager::startTerminalEventLoop() {
    startEventLoop(true);
  }

  void EventManager::startEventLoop(bool readFromTerminal) {
    init_gui_event_stream(_eventStreamFDs);

    io_descriptor waitFDs[4];
    waitFDs[0]._fd = _eventStreamFDs[0];
    waitFDs[0]._ioType = IO_OP_TYPES::IO_Read;

    waitFDs[1]._fd = _eventStreamFDs[1];
    waitFDs[1]._ioType = IO_OP_TYPES::IO_Read;

    waitFDs[2]._fd = -1;
    waitFDs[3]._fd = -1;
    if (readFromTerminal) {
      waitFDs[2]._fd = STDIN_FD;
      waitFDs[2]._ioType = IO_OP_TYPES::IO_Read;
    }

    io_descriptor readyFDs[4];
    readyFDs[0]._fd = -1;

    try {
      while (true) {
        select(waitFDs, readyFDs);

        for(int i = 0; readyFDs[i]._fd >= 0; ++i) {
          if (readyFDs[i]._fd == _eventStreamFDs[0]) { //Keyboard
            handleKeyboardEvent(_eventStreamFDs[0]);
          } else if (readyFDs[i]._fd == _eventStreamFDs[1]) { //Mouse
            handleMouseEvent(_eventStreamFDs[1]);
          } else if(readyFDs[i]._fd == STDIN_FD) {
            handleTerminalInput(STDIN_FD);
          }
        }
      }
    } catch(upan::exception& e) {
      e.Print();
    }
  }

  KeyboardData EventManager::getCh() {
    io_descriptor waitFDs[2];
    waitFDs[0]._fd = _eventStreamFDs[0];
    waitFDs[0]._ioType = IO_OP_TYPES::IO_Read;

    waitFDs[1]._fd = -1;

    io_descriptor readyFDs[2];
    readyFDs[0]._fd = -1;

    select(waitFDs, readyFDs);

    KeyboardData data;
    auto n = read(_eventStreamFDs[0], (void*)&data, sizeof(KeyboardData));
    if (n != sizeof(KeyboardData)) {
      throw upan::exception(XLOC, "read event data size (%d) < RawKeyboardData size (%d)", n, sizeof(KeyboardData));
    }

    return data;
  }

  void EventManager::handleKeyboardEvent(int fd) {
    while(true) {
      KeyboardData data;
      auto n = read(fd, (void*)&data, sizeof(KeyboardData));
      if (n == 0) {
        return;
      }
      if (n != sizeof(KeyboardData)) {
        throw upan::exception(XLOC, "read event data size (%d) < KeyboardData size (%d)", n, sizeof(KeyboardData));
      }

      KeyboardEvent keyboardEvent(data);
      GraphicsContext::Instance().uiObjectManager().dispatch(keyboardEvent);
    }
  }

  void EventManager::handleMouseEvent(int fd) {
    while(true) {
      MouseData data;
      auto n = read(fd, (void*)&data, sizeof(MouseData));
      if (n == 0) {
        return;
      }
      if (n != sizeof(MouseData)) {
        throw upan::exception(XLOC, "read event data size (%d) < MouseData size (%d)", n, sizeof(MouseData));
      }

      GraphicsContext::Instance().uiObjectManager().dispatch(data);
    }
  }

  void EventManager::handleTerminalInput(int fd) {
  	const int MAX_BUFFER_SIZE = 1024;
    uint8_t buffer[MAX_BUFFER_SIZE];
    int n = read(fd, buffer, MAX_BUFFER_SIZE);
    if (n > 0) {
      for (int i = 0; i < n; ++i) {
        KeyboardData data(buffer[i], false, false, false);
        KeyboardEvent keyboardEvent(data);
        GraphicsContext::Instance().uiObjectManager().dispatch(keyboardEvent);
      }
    }
  }
}