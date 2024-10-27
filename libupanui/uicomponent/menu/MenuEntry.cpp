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

#include <MenuEntry.h>
#include <Menu.h>
#include <MouseEventHandler.h>
#include <UIObjectFactory.h>
#include <UIRoot.h>

namespace upanui {
  MenuEntry::MenuEntry(UIRoot& uiRoot, int id, const upan::string& name, Menu::ActionHandler& handler,
                       int x, int y, int width, int height)
    : RectangleCanvas(x, y, width, height, HorizontalPlacementType::ABSOLUTE, VerticalPlacementType::ABSOLUTE),
      _uiRoot(uiRoot), _id(id), _name(name), _handler(handler), _uiLabel(nullptr) {
    UIObjectImpl::captureMouseEvents(true);
  }

  void MenuEntry::init() {
    backgroundColor(parent().backgroundColor());
    _uiLabel = &UIObjectFactory::createLabel(*this,
                                             0, 0,
                                             width(), height(),
                                             upan::string(" ") + _name, 0,
                                             upanui::usfn::PreloadedFonts::VGA16,
                                             upanui::usfn::FAMILY_MONOSPACE, upanui::usfn::STYLE_REGULAR, Menu::MENU_LABEL_FONT_SIZE,
                                             Label::HorizontalTextAlignment::LEFT, Label::VerticalTextAlignment::VCENTER,
                                             upanui::HorizontalPlacementType::ABSOLUTE, upanui::VerticalPlacementType::ABSOLUTE);
    _uiLabel->backgroundColorAlpha(0);
  }

  void MenuEntry::onMouseEvent(const MouseEvent& event) {
    if (event.getData().leftButtonState() == MouseData::State::PRESSED) {
      _handler.invoke(_id, _name);
      _uiRoot.closeActiveMenu();
    }
    RectangleCanvas::onMouseEvent(event);
  }

  void MenuEntry::onMouseFocus() {
    backgroundColor(0x0000FF);
    _uiLabel->setFGColor(0xFFFFFF);
    notifyChange(ChangeState::Content);
  }

  void MenuEntry::onLoseMouseFocus() {
    backgroundColor(parent().backgroundColor());
    _uiLabel->setFGColor(0);
    notifyChange(ChangeState::Content);
  }
}