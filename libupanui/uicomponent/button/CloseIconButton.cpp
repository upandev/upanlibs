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

#include <CloseIconButton.h>
#include <UIObjectFactory.h>

namespace upanui {
  CloseIconButton::CloseIconButton(const int x, const int y, const uint32_t width, const uint32_t height)
    : Button(x, y, width, height) {
  }

  void CloseIconButton::init() {
    {
      ChangeNotificationLock g(*this);
      hoverColor(0xD53C1C);
      clickColor(0xC56043);
      backgroundColor(0x91908D);

      const int dx = width() * 0.3;
      const int dy = height() * 0.3;

      auto &line1 = UIObjectFactory::createLine(*this, dx, dy, width() - dx + 2, height() - dy - 3, 2);
      line1.backgroundColor(0xFFFFFF);

      //auto &line2 = UIObjectFactory::createLine(*this, dx, height() - dy, width() - dx, dy, 2);
      //line2.backgroundColor(0xFFFFFF);
    }
    contentChanged();
  }
}