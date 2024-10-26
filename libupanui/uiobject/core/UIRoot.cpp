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

#include <UIRoot.h>
#include <GraphicsContext.h>
#include <UIObjectFactory.h>
#include <Menu.h>
#include <MenuEntry.h>
#include <IconButton.h>
#include <typeinfo.h>

namespace upanui {
  UIRoot::UIRoot(int x, int y, int width, int height)
          : UIObjectImpl(x, y, width, height, HorizontalPlacementType::STRETCHED, VerticalPlacementType::STRETCHED),
            _layout(*this), _menuInitialized(false), _menuBarHeight(0), _activeMenu(nullptr) {
    gc().frame().updateViewport(x, y, width, height);
    UIObjectImpl::drawBuffer().initLocal(gc().frame().frameBuffer());
  }

  void UIRoot::draw() {
    layout().fill();
    for (auto& child: children()) {
      child->drawTopDown();
    }
  }

  void UIRoot::drawTopDown() {
    throw upan::exception(XLOC, "unsupported drawTopDown() because UIRoot can't be a child element");
  }

  void UIRoot::drawToTop() {
    //no-op
  }

  int UIRoot::drawX() const {
    return 0;
  }

  int UIRoot::drawY() const {
    return 0;
  }

  UIObjectImpl::IntersectInfo UIRoot::intersect(int x, int y) const {
    const auto bgAlpha = backgroundColorAlpha();

    const int x1 = drawX();
    const int x2 = drawX() + width();
    const int y1 = drawY();
    const int y2 = drawY() + height();

    return { intersectWithAlpha(bgAlpha), x - x1, x2 - x, y - y1, y2 - y };
  }

  void UIRoot::notifyChange(const ChangeState changeState) {
    if (isChangeNotificationLocked()) {
      return;
    }

    setChangeState(changeState);
    switch(changeState) {
      case ChangeState::Position:
        gc().frame().updateViewport(x(), y(), width(), height());
        break;
      case ChangeState::Size:
        //gc().frame().updateViewport(x(), y(), width(), height());
        redraw();
        break;
      case ChangeState::Content:
        redraw();
        break;
    }
  }

  void UIRoot::updateViewport() {
    GraphicsContext::Instance().frame().updateViewport(x(), y(), width(), height());
  }

  int UIRoot::resizeLeft(int dx, bool isPrimary) {
    if (dx != 0) {
      dx = resizeWidth(width() - dx, isPrimary);
      x(x() + dx);
    }
    return dx;
  }

  int UIRoot::resizeRight(int dx, bool isPrimary) {
    if (dx != 0) {
      dx = -resizeWidth(width() + dx, isPrimary);
    }
    return dx;
  }

  int UIRoot::resizeTop(int dy, bool isPrimary) {
    if (dy != 0) {
      dy = resizeHeight(height() - dy, isPrimary);
      y(y() + dy);
    }
    return dy;
  }

  int UIRoot::resizeBottom(int dy, bool isPrimary) {
    if (dy != 0) {
      dy = -resizeHeight(height() + dy, isPrimary);
    }
    return dy;
  }

  bool UIRoot::isVisible() const {
    return getVisible();
  }

  class AppDragMouseHandler : public upanui::MouseEventHandler {
  public:
    void onEvent(upanui::UIObject& uiObject, const upanui::MouseEvent& event) override {
      const upanui::MouseData& data = event.getData();
      if (data.leftButtonState() == upanui::MouseData::HOLD) {
        uiObject.parent().xy(uiObject.parent().x() + data.deltaX(), uiObject.parent().y() + data.deltaY());
      }
    }
  };

  class CloseButtonMouseHandler : public upanui::MouseEventHandler {
  public:
    void onEvent(upanui::UIObject& uiObject, const upanui::MouseEvent& event) override {
      const upanui::MouseData& data = event.getData();
      if (data.leftButtonState() == upanui::MouseData::RELEASED) {
        exit(0);
      }
    }
  };

  void UIRoot::initMenuBar() {
    if (_menuInitialized) {
      return;
    }
    _menuInitialized = true;

    _menuBarHeight = 30;//Menu::MENU_LABEL_FONT_SIZE + Menu::MENU_LABEL_FONT_SIZE / 2;
    const int appWidth = width();

    auto& uiMenuBar = UIObjectFactory::createRectangleCanvas(*this, 0, 0, appWidth, _menuBarHeight, upanui::HorizontalPlacementType::STRETCHED, upanui::VerticalPlacementType::TOP_FIXED);
    uiMenuBar.backgroundColor(0xA59E9D);

    auto& fileMenu = upanui::UIObjectFactory::createMenu(*this, uiMenuBar, "File", 0, 0, _menuBarHeight);
    auto& editMenu = upanui::UIObjectFactory::createMenu(*this, uiMenuBar, "Edit", fileMenu.width(), 0, _menuBarHeight);

    auto& closeBt = upanui::UIObjectFactory::createIconButton(uiMenuBar, upanui::PngImageResource::CLOSE, appWidth - _menuBarHeight, 0, _menuBarHeight, _menuBarHeight, upanui::HorizontalPlacementType::RIGHT_FIXED, upanui::VerticalPlacementType::TOP_FIXED);

    static AppDragMouseHandler appDragMouseHandler;
    uiMenuBar.registerMouseEventHandler(appDragMouseHandler);

    static CloseButtonMouseHandler closeButtonMouseHandler;
    closeBt.registerMouseEventHandler(closeButtonMouseHandler);
  }

  void UIRoot::onMenuClick(upanui::Menu& menu) {
    if (_activeMenu == &menu) {
      closeActiveMenu();
    } else {
      closeActiveMenu();
      _activeMenu = &menu;
      _activeMenu->select(true);
    }
  }

  void UIRoot::onMenuHover(upanui::Menu& menu) {
    if (_activeMenu) {
      if (_activeMenu != &menu) {
        closeActiveMenu();
        _activeMenu = &menu;
        _activeMenu->select(true);
      }
    }
  }

  void UIRoot::onMenuEntryClick(int id) {
    //call-back action for id
    closeActiveMenu();
  }

  void UIRoot::closeActiveMenu() {
    if (_activeMenu) {
      _activeMenu->select(false);
      _activeMenu = nullptr;
    }
  }

  void UIRoot::drawActiveMenu() {
    if (_activeMenu) {
      _activeMenu->drawPanel();
    }
  }

  bool UIRoot::isModelActive() const {
    return _activeMenu != nullptr;
  }

  void UIRoot::handleMouseEvent(const MouseEvent& e, const UIObject& eo) {
    if (e.getData().anyButtonPressed()) {
      if (typeid(eo) != typeid(Menu) && typeid(eo) != typeid(MenuEntry)) {
        closeActiveMenu();
      }
    }
  }
}
