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

#include <stdint.h>
#include <ustring.h>
#include <usfntypes.h>
#include <ImageResource.h>

namespace upanui {
  class UIObject;
  class RectangleCanvas;
  class RoundCanvas;
  class ImageCanvas;
  class Image;
  class Line;
  class Label;

  class Button;
  class IconButton;

  class VerticalScroller;

  class UIObjectFactory {
  public:
    static RectangleCanvas& createRectangleCanvas(UIObject& parent, const int x, const int y, const uint32_t width, const uint32_t height);
    static RoundCanvas& createRoundCanvas(UIObject& parent, const int x, const int y, const uint32_t width, const uint32_t height);

    static ImageCanvas& createImageCanvas(UIObject& parent, const Image& image, ImageComposeType composeType,
                                          const int x, const int y, const uint32_t width, const uint32_t height);
    static ImageCanvas& createImageCanvas(UIObject& parent, const Image& image,
                                          const int x, const int y, const uint32_t width, const uint32_t height);
    ImageCanvas& createImageCanvas(UIObject& parent, const Image& image, const int x, const int y);
    static ImageCanvas& createImageCanvas(UIObject& parent, const ImageResource& imageResource, ImageComposeType composeType,
                                          const int x, const int y, const uint32_t width, const uint32_t height);
    static ImageCanvas& createImageCanvas(UIObject& parent, const ImageResource& imageResource,
                                          const int x, const int y, const uint32_t width, const uint32_t height);
    static ImageCanvas& createImageCanvas(UIObject& parent, const ImageResource& imageResource, const int x, const int y);

    static Line& createLine(UIObject& parent, const int x1, const int y1, const int x2, const int y2, const uint32_t thickness);

    static Button& createButton(UIObject& parent, const int x, const int y, const uint32_t width, const uint32_t height);
    static IconButton& createIconButton(UIObject& parent, const ImageResource& imageResource, ImageComposeType composeType,
                                        const int x, const int y, const uint32_t width, const uint32_t height);
    static IconButton& createIconButton(UIObject& parent, const ImageResource& imageResource,
                                        const int x, const int y, const uint32_t width, const uint32_t height);

    static Label& createLabel(UIObject& parent, const int x, const int y,
                              const uint32_t width, const uint32_t height,
                              const upan::string& str, uint32_t fgColor,
                              usfn::PreloadedFonts fontType,
                              int fontFamily, int fontStyle, int fontSize);

    static VerticalScroller& createVerticalScroller(UIObject& parent, const int x, const int y,
                                                    const uint32_t width, const uint32_t height,
                                                    const uint32_t scrollBarWidth, const uint32_t scrollBarHeight);
  };
}