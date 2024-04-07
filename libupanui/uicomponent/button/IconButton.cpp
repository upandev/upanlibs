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

#include <IconButton.h>
#include <UIObjectFactory.h>
#include <Line.h>

namespace upanui {
  IconButton::IconButton(const ImageResource& imageResource, const ImageComposeType composeType,
                         int x, int y,
                         int width, int height)
    : Button(x, y, width, height), _imageResource(imageResource), _composeType(composeType) {
  }

  void IconButton::init() {
    {
      ChangeNotificationLock g(*this);
      hoverColor(0xD53C1C);
      clickColor(0xC56043);
      backgroundColor(0x91908D);

      //reduce image size by 40% - this should be parameterized
      const auto iw = width() * 0.6;
      const auto ih = height() * 0.6;
      // center the image
      const int ix = (width() - iw) / 2;
      const int iy = (height() - ih) / 2;

      UIObjectFactory::createImageCanvas(*this, _imageResource, _composeType, ix, iy, iw, ih);
    }
    notifyChange(ChangeState::Content);
  }
}