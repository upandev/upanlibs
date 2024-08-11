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

#include <IconLabel.h>
#include <UIObjectFactory.h>
#include <Label.h>

namespace upanui {
  IconLabel::IconLabel(const Image& image, const upan::string& label,
                       int x, int y,
                       int width, int height,
                       HorizontalPlacementType horizontalPlacementType,
                       VerticalPlacementType verticalPlacementType)
    : RectangleCanvas(x, y, width, height, horizontalPlacementType, verticalPlacementType),
      _image(image), _label(label), _uiLabel(nullptr), _selected(false) {
  }

  void IconLabel::init() {
    {
      ChangeNotificationLock g(*this);
      backgroundColorAlpha(0);
      borderColorAlpha(0);
      borderThickness(1);
      borderColor(0xFFFFFF);

      const int awidth = width() - 2 * borderThickness();
      const int aheight = height() - 2 * borderThickness();
      const int labelHeight = 20;

      if ((labelHeight + 2 * labelHeight) > aheight) {
        throw upan::exception(XLOC, "IconLabel height is too small");
      }

      const int imageHeight = aheight - labelHeight;
      if (imageHeight > awidth) {
        throw upan::exception(XLOC, "IconLabel width is too small");
      }

      const auto imageWidth = imageHeight;

      // center the image
      const int ix = (awidth - imageWidth) / 2;

      UIObjectFactory::createImageCanvas(*this, _image, ImageComposeType::FIT_IN,
                                         ix, 0,
                                         imageWidth, imageHeight,
                                         HorizontalPlacementType::ABSOLUTE, VerticalPlacementType::ABSOLUTE);

      const int fontSize = 16;
      _uiLabel = &UIObjectFactory::createLabel(*this,
                                   0, imageHeight,
                                   awidth, labelHeight,
                                   _label, 0xFFFFFF,
                                   upanui::usfn::PreloadedFonts::VGA16,
                                   upanui::usfn::FAMILY_MONOSPACE, upanui::usfn::STYLE_REGULAR, fontSize,
                                   Label::HorizontalTextAlignment::HCENTER, Label::VerticalTextAlignment::VCENTER,
                                   upanui::HorizontalPlacementType::ABSOLUTE, upanui::VerticalPlacementType::ABSOLUTE);
      _uiLabel->backgroundColorAlpha(0);
      _uiLabel->backgroundColor(0x0000FF);
    }
    notifyChange(ChangeState::Content);
  }

  void IconLabel::select(bool selected) {
    if (selected != _selected) {
      borderColorAlpha(selected ? 0xFF : 0);
      _uiLabel->backgroundColorAlpha(selected ? 0xFF : 0);
      _selected = selected;
      notifyChange(ChangeState::Content);
    }
  }
}