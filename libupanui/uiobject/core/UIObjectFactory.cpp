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
#include <IconButton.h>
#include <Label.h>
#include <VerticalScroller.h>
#include <Image.h>
#include <TextArea.h>
#include <typeinfo.h>

namespace upanui {
  RectangleCanvas& UIObjectFactory::createRectangleCanvas(UIObject& parent, int x, int y, int width, int height) {
    auto& canvas = *new RectangleCanvas(x, y, width, height);
    parent.add(canvas);
    return canvas;
  }

  RoundCanvas& UIObjectFactory::createRoundCanvas(UIObject& parent, int x, int y, int width, int height) {
    auto& canvas = *new RoundCanvas(x, y, width, height);
    parent.add(canvas);
    return canvas;
  }

  ImageCanvas& UIObjectFactory::createImageCanvas(UIObject& parent, const Image& image, ImageComposeType composeType,
                                                  int x, int y, int width, int height) {
    auto& canvas = *new ImageCanvas(image, composeType, x, y, width, height);
    parent.add(canvas);
    return canvas;
  }

  ImageCanvas& UIObjectFactory::createImageCanvas(UIObject& parent, const Image& image, int x, int y, int width, int height) {
    return createImageCanvas(parent, image, ImageComposeType::FIT_IN, x, y, width, height);
  }

  ImageCanvas& UIObjectFactory::createImageCanvas(UIObject& parent, const Image& image, int x, int y) {
    return createImageCanvas(parent, image, ImageComposeType::FIT_IN, x, y, image.width(), image.height());
  }

  ImageCanvas& UIObjectFactory::createImageCanvas(UIObject& parent, const ImageResource& imageResource, ImageComposeType composeType,
                                                  int x, int y, int width, int height) {
    auto& canvas = *new ImageCanvas(&imageResource.create(), composeType, x, y, width, height);
    parent.add(canvas);
    return canvas;
  }

  ImageCanvas& UIObjectFactory::createImageCanvas(UIObject& parent, const ImageResource& imageResource,
                                                  int x, int y, int width, int height) {
    return createImageCanvas(parent, imageResource, ImageComposeType::FIT_IN, x, y, width, height);
  }

  ImageCanvas& UIObjectFactory::createImageCanvas(UIObject& parent, const ImageResource& imageResource, int x, int y) {
    Image& image = imageResource.create();
    auto& canvas = *new ImageCanvas(&image, ImageComposeType::FIT_IN, x, y, image.width(), image.height());
    parent.add(canvas);
    return canvas;
  }

  Line& UIObjectFactory::createLine(UIObject& parent, int x1, int y1, int x2, int y2, int thickness) {
    auto& line = *new Line(x1, y1, x2, y2, thickness);
    parent.add(line);
    return line;
  }

  Button& UIObjectFactory::createButton(UIObject& parent, int x, int y, int width, int height) {
    auto& button = *new Button(x, y, width, height);
    parent.add(button);
    return button;
  }

  IconButton& UIObjectFactory::createIconButton(UIObject& parent, const ImageResource& imageResource, ImageComposeType composeType,
                                                int x, int y, int width, int height) {
    auto& button = *new IconButton(imageResource, composeType, x, y, width, height);
    parent.add(button);
    button.init();
    return button;
  }

  IconButton& UIObjectFactory::createIconButton(UIObject& parent, const ImageResource& imageResource,
                                                int x, int y, int width, int height) {
    return createIconButton(parent, imageResource, ImageComposeType::FIT_IN, x, y, width, height);
  }

  Label& UIObjectFactory::createLabel(UIObject& parent, int x, int y,
                                      int width, int height,
                                      const upan::string& str, uint32_t fgColor,
                                      usfn::PreloadedFonts fontType,
                                      int fontFamily, int fontStyle, int fontSize) {
    auto& label = *new Label(x, y, width, height, str, fgColor, fontType, fontFamily, fontStyle, fontSize);
    parent.add(label);
    return label;
  }

  VerticalScroller& UIObjectFactory::createVerticalScroller(UIObject& parent, int x, int y,
                                                            int width, int height,
                                                            int scrollBarWidth) {
    auto& scroller = *new VerticalScroller(x, y, width, height, scrollBarWidth);
    parent.add(scroller);
    scroller.init();
    return scroller;
  }

  TextArea& UIObjectFactory::createTextArea(UIObject& parent, int x, int y, int width, int height) {
    int actualHeight = height;
    if (typeid(parent) == typeid(VerticalScroller)) {
      actualHeight = parent.height();
    }
    auto& textArea = *new TextArea(x, y, width, actualHeight);
    parent.add(textArea);
    textArea.init(0);
    return textArea;
  }

}