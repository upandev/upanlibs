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
#include <fs.h>
#include <mosstd.h>

namespace upanui {
  EventManager::EventManager() {
    _eventHandlers.insert(EventHandlerMap::value_type(Keyboard, upan::list<EventHandler*>()));
    _eventHandlers.insert(EventHandlerMap::value_type(Mouse, upan::list<EventHandler*>()));
  }

  void EventManager::registerKeyboardEventHandler(KeyboardEventHandler& handler) {
    _eventHandlers[Keyboard].push_back(&handler);
  }

  void EventManager::registerMouseEventHandler(MouseEventHandler &handler) {
    _eventHandlers[Keyboard].push_back(&handler);
  }

  void EventManager::startEventLoop() {
    init_gui_event_stream(_eventStreamFDs);

    io_descriptor waitFDs[3];
    waitFDs[0]._fd = _eventStreamFDs[0];
    waitFDs[0]._ioType = IO_OP_TYPES::IO_Read;

    waitFDs[1]._fd = _eventStreamFDs[1];
    waitFDs[1]._ioType = IO_OP_TYPES::IO_Read;

    waitFDs[2]._fd = -1;

    io_descriptor readyFDs[3];
    readyFDs[0]._fd = -1;

    try {
      while (true) {
        select(waitFDs, readyFDs);

        for(int i = 0; readyFDs[i]._fd >= 0; ++i) {
          if (readyFDs[i]._fd == _eventStreamFDs[0]) { //Keyboard
            handleKeyboardEvent(_eventStreamFDs[0]);
          } else if (readyFDs[i]._fd == _eventStreamFDs[1]) { //Mouse

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
        throw upan::exception(XLOC, "read event data size (%d) < RawKeyboardData size (%d)", n, sizeof(KeyboardData));
      }

      KeyboardEvent keyboardEvent(data);

      for(auto handler : _eventHandlers[Keyboard]) {
        if (handler->isFocused()) {
          handler->dispatch(keyboardEvent);
        }
      }
    }
  }
}