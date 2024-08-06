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
#include <UIEnums.h>
#include <Terminal.h>

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
  class TextArea;

  class UIObjectFactory {
  public:
    static RectangleCanvas& createRectangleCanvas(UIObject& parent, int x, int y, int width, int height,
                                                  HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    static RoundCanvas& createRoundCanvas(UIObject& parent, int x, int y, int width, int height,
                                          HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);

    static ImageCanvas& createImageCanvas(UIObject& parent, const Image& image, ImageComposeType composeType,
                                          int x, int y, int width, int height,
                                          HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    static ImageCanvas& createImageCanvas(UIObject& parent, const Image& image,
                                          int x, int y, int width, int height,
                                          HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    ImageCanvas& createImageCanvas(UIObject& parent, const Image& image, int x, int y,
                                   HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    static ImageCanvas& createImageCanvas(UIObject& parent, const ImageResource& imageResource, ImageComposeType composeType,
                                          int x, int y, int width, int height,
                                          HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    static ImageCanvas& createImageCanvas(UIObject& parent, const ImageResource& imageResource,
                                          int x, int y, int width, int height,
                                          HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    static ImageCanvas& createImageCanvas(UIObject& parent, const ImageResource& imageResource, int x, int y,
                                          HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);

    static Line& createLine(UIObject& parent, int x1, int y1, int x2, int y2, int thickness,
                            HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);

    static Button& createButton(UIObject& parent, int x, int y, int width, int height,
                                HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    static IconButton& createIconButton(UIObject& parent, const ImageResource& imageResource, ImageComposeType composeType,
                                        int x, int y, int width, int height,
                                        HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    static IconButton& createIconButton(UIObject& parent, const ImageResource& imageResource,
                                        int x, int y, int width, int height,
                                        HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);

    static Label& createLabel(UIObject& parent, int x, int y,
                              int width, int height,
                              const upan::string& str, uint32_t fgColor,
                              usfn::PreloadedFonts fontType,
                              int fontFamily, int fontStyle, int fontSize,
                              HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);

    static VerticalScroller& createVerticalScroller(UIObject& parent, int x, int y,
                                                    int width, int height,
                                                    int scrollBarWidth,
                                                    HorizontalPlacementType horizontalPlacementType,
                                                    VerticalPlacementType verticalPlacementType);
    static TextArea& createTextArea(UIObject& parent, int x, int y, int width, int height,
                                    HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
    static Terminal& createTerminal(UIObject& parent, int x, int y, int width, int height,
                                    const upan::string& prompt,
                                    Terminal::CommandExecutor& commandExecutor,
                                    HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType);
  };
}