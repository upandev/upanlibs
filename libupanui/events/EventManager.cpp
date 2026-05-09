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
#include <sys/select.h>

namespace upanui {
  EventManager::EventManager() {
  }

  int EventManager::startEventLoop() {
    init_gui_event_stream(_eventStreamFDs);

    int exit_fds[2];
    if (pipe(exit_fds) < 0) {
      throw upan::exception(XLOC, "failed to create exit pipe");
    }
    _exitPipeFD = exit_fds[1];

    fd_set readfds;
    const int nfds = upan::max(_eventStreamFDs[0], upan::max(_eventStreamFDs[1], exit_fds[0])) + 1;

    try {
      while (true) {
        FD_ZERO(&readfds);
        FD_SET(_eventStreamFDs[0], &readfds);
        FD_SET(_eventStreamFDs[1], &readfds);
        FD_SET(exit_fds[0], &readfds);

        if (select(nfds, &readfds, NULL, NULL, NULL) >= 1) {
          if (FD_ISSET(_eventStreamFDs[0], &readfds)) {
            handleKeyboardEvent(_eventStreamFDs[0]);
          }
          if (FD_ISSET(_eventStreamFDs[1], &readfds)) {
            handleMouseEvent(_eventStreamFDs[1]);
          }
          if (FD_ISSET(exit_fds[0], &readfds)) {
            int exitCode;
            auto n = read(exit_fds[0], (void*)&exitCode, sizeof(int));
            if (n == 0) {
              continue;
            }
            if (n != sizeof(int)) {
              return -1;
            }
            return exitCode;
          }
        }
      }
    } catch(upan::exception& e) {
      e.Print();
    }
  }

  KeyboardData EventManager::getCh() {
    fd_set readfds;
    const int nfds = _eventStreamFDs[0] + 1;
    FD_ZERO(&readfds);
    FD_SET(_eventStreamFDs[0], &readfds);

    while (select(nfds, &readfds, NULL, NULL, NULL) < 1);
    KeyboardData data;
    auto n = read(_eventStreamFDs[0], (void*) &data, sizeof(KeyboardData));
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

  void EventManager::stopEventLoop(int exitCode) {
    write(_exitPipeFD, (void*)&exitCode, sizeof(int));
  }
}