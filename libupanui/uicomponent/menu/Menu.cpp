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
    : RectangleCanvas(x, y, calcWidth(title), height, HorizontalPlacementType::ABSOLUTE, VerticalPlacementType::ABSOLUTE),
      _uiRoot(uiRoot), _title(title), _uiLabel(nullptr) {
    UIObjectImpl::captureMouseEvents(true);
  }

  void Menu::init() {
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

    _uiPanel = &UIObjectFactory::createRectangleCanvas(_uiRoot, x(), height(), 50, 100,
                                                       upanui::HorizontalPlacementType::ABSOLUTE, upanui::VerticalPlacementType::ABSOLUTE);
    _uiPanel->backgroundColor(0xD2D1C8);
    _uiPanel->captureMouseEvents(true);

    auto me1 = new MenuEntry(_uiRoot, "New", 5, 5, 40, 30);
    _uiPanel->add(*me1);
    me1->init();

    auto me2 = new MenuEntry(_uiRoot, "Open", 5, 50, 40, 30);
    _uiPanel->add(*me2);
    me2->init();

    auto me3 = new MenuEntry(_uiRoot, "Exit", 5, 95, 40, 30);
    _uiPanel->add(*me3);
    me3->init();
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