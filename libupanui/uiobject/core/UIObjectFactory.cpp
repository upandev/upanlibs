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

#include <UIObjectFactory.h>
#include <Button.h>
#include <RoundCanvas.h>
#include <ImageCanvas.h>
#include <Line.h>
#include <CloseIconButton.h>
#include <Label.h>

namespace upanui {
  RectangleCanvas& UIObjectFactory::createRectangleCanvas(UIObject& parent, const int x, const int y, const uint32_t width, const uint32_t height) {
    auto& canvas = *new RectangleCanvas(x, y, width, height);
    parent.add(canvas);
    return canvas;
  }

  RoundCanvas& UIObjectFactory::createRoundCanvas(UIObject& parent, const int x, const int y, const uint32_t width, const uint32_t height) {
    auto& canvas = *new RoundCanvas(x, y, width, height);
    parent.add(canvas);
    return canvas;
  }

  ImageCanvas& UIObjectFactory::createImageCanvas(UIObject& parent, const Image& image, const int x, const int y, const uint32_t width, const uint32_t height) {
    auto& canvas = *new ImageCanvas(image, x, y, width, height);
    parent.add(canvas);
    return canvas;
  }

  Line& UIObjectFactory::createLine(UIObject& parent, const int x1, const int y1, const int x2, const int y2, const uint32_t thickness) {
    auto& line = *new Line(x1, y1, x2, y2, thickness);
    parent.add(line);
    return line;
  }

  Button& UIObjectFactory::createButton(UIObject& parent, const int x, const int y, const uint32_t width, const uint32_t height) {
    auto& button = *new Button(x, y, width, height);
    parent.add(button);
    return button;
  }

  CloseIconButton& UIObjectFactory::createCloseIconButton(UIObject& parent, const int x, const int y, const uint32_t width, const uint32_t height) {
    auto& button = *new CloseIconButton(x, y, width, height);
    parent.add(button);
    button.init();
    return button;
  }

  Label& UIObjectFactory::createLabel(UIObject& parent, const int x, const int y,
                                      const uint32_t width, const uint32_t height,
                                      const upan::string& str, uint32_t fgColor,
                                      usfn::PreloadedFonts fontType,
                                      int fontFamily, int fontStyle, int fontSize) {
    auto& label = *new Label(x, y, width, height, str, fgColor, fontType, fontFamily, fontStyle, fontSize);
    parent.add(label);
    return label;
  }
}