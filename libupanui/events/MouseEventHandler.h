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

#include <EventHandler.h>
#include <exception.h>
#include <MouseEvent.h>

namespace upanui {
class MouseEventHandler : public EventHandler {
public:
  MouseEventHandler(UIObject& uiObject) : EventHandler(uiObject) {}

  virtual void onEvent(MouseEvent& event) = 0;

private:
  void dispatch(Event& event) override {
    auto e = dynamic_cast<MouseEvent*>(&event);
    if (e == nullptr) {
      throw upan::exception(XLOC, "Event is not MouseEvent. It's: %s", typeid(&event).name());
    }
    onEvent(*e);
  }
};
}