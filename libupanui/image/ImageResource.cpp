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
#include <ImageResource.h>

#define DECLARE_IMAGE(name, type) \
extern unsigned _binary_icons_##type##_##name##_##type##_start; \
extern unsigned _binary_icons_##type##_##name##_##type##_size;

#define IMAGE_PARAMS(name, type) \
&_binary_icons_##type##_##name##_##type##_start, (size_t)&_binary_icons_##type##_##name##_##type##_size

DECLARE_IMAGE(test, png)
DECLARE_IMAGE(mouse_cursor, png)

DECLARE_IMAGE(mouse_cursor, bmp)

namespace upanui {
  const ImageResource ImageResource::TEST_PNG(IMAGE_PARAMS(test, png));
  const ImageResource ImageResource::MOUSE_CURSOR_PNG(IMAGE_PARAMS(mouse_cursor, png));

  const ImageResource ImageResource::MOUSE_CURSOR_BMP(IMAGE_PARAMS(mouse_cursor,bmp));
}
