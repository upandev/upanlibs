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

#include <Menu.h>
#include <MouseEventHandler.h>
#include <UIObjectFactory.h>
#include <UIRoot.h>
#include <MenuEntry.h>

namespace upanui {
  static int calcWidth(const upan::string& title) {
    return Menu::MENU_LABEL_FONT_SIZE / 2 * title.length() + Menu::MENU_LABEL_FONT_SIZE;
  }

  Menu::Menu(UIRoot& uiRoot, const upan::string& title, int x, int y, int height)
    : RectangleCanvas(x, y, calcWidth(title), height,
                      HorizontalPlacementType::LEFT_FIXED, VerticalPlacementType::TOP_FIXED),
      _uiRoot(uiRoot), _title(title), _uiLabel(nullptr) {
    UIObjectImpl::captureMouseEvents(true);
  }

  void Menu::init(const upan::list<MenuEntryInfo>& menuEntryInfo) {
    backgroundColor(parent().backgroundColor());
    _uiLabel = &UIObjectFactory::createLabel(*this,
                                             0, 0,
                                             width(), height(),
                                             _title, 0,
                                             upanui::usfn::PreloadedFonts::VGA16,
                                             upanui::usfn::FAMILY_MONOSPACE, upanui::usfn::STYLE_REGULAR, MENU_LABEL_FONT_SIZE,
                                             Label::HorizontalTextAlignment::HCENTER, Label::VerticalTextAlignment::VCENTER,
                                             upanui::HorizontalPlacementType::ABSOLUTE, upanui::VerticalPlacementType::ABSOLUTE);
    _uiLabel->backgroundColorAlpha(0);

    const int menuEntryHeight = 30;
    _uiPanel = &UIObjectFactory::createRectangleCanvas(_uiRoot, 1, x(), height(), 100, menuEntryInfo.size() * menuEntryHeight,
                                                       upanui::HorizontalPlacementType::LEFT_FIXED,
                                                       upanui::VerticalPlacementType::TOP_FIXED);
    _uiPanel->backgroundColor(0xD2D1C8);
    _uiPanel->captureMouseEvents(true);
    _uiPanel->setVisible(false);

    int menuEntryY = 0;
    for(auto& i : menuEntryInfo) {
      UIObjectFactory::createMenuEntry(_uiRoot, *_uiPanel, i.id(), i.name(), i.handler(), 0, menuEntryY,
                                       _uiPanel->width(), menuEntryHeight);
      menuEntryY += menuEntryHeight;
    }
  }

  void Menu::onKeyboardEvent(const KeyboardEvent& event) {
  }

  void Menu::select(bool selected) {
    backgroundColor(selected ? 0xD2D1C8 : parent().backgroundColor());
    _uiPanel->setVisible(selected);
    notifyChange(ChangeState::Content);
  }

  void Menu::onMouseEvent(const MouseEvent& event) {
    if (event.getData().leftButtonState() == MouseData::State::PRESSED) {
      _uiRoot.onMenuClick(*this);
    }
    RectangleCanvas::onMouseEvent(event);
  }

  void Menu::onMouseFocus() {
    _uiRoot.onMenuHover(*this);
  }

  void Menu::drawPanel() {
    static_cast<UIObject*>(_uiPanel)->draw();
  }
}