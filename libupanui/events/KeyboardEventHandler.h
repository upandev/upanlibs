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

#include <EventHandler.h>
#include <exception.h>
#include <typeinfo.h>
#include <KeyboardEvent.h>

namespace upanui {
class Event;
class KeyboardEvent;

class KeyboardEventHandler : public EventHandler {
public:
  KeyboardEventHandler(UIObject& uiObject) : EventHandler(uiObject) {}

  virtual void onEvent(KeyboardEvent& event) = 0;

private:
  void dispatch(Event& event) override {
    auto e = dynamic_cast<KeyboardEvent*>(&event);
    if (e == nullptr) {
      throw upan::exception(XLOC, "Event is not KeyboardEvent. It's: %s", typeid(&event).name());
    }
    onEvent(*e);
  }
};
}