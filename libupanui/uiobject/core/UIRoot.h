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

#include <stdlib.h>
#include <UIObjectImpl.h>
#include <RectangularLayout.h>
#include <Menu.h>

namespace upanui {
  class UIRoot : public UIObjectImpl {
  public:
    class MenuInfo {
    public:
      MenuInfo(const upan::string& title, const upan::list<Menu::MenuEntryInfo>& menuEntryInfo)
      : _title(title), _menuEntryInfo(menuEntryInfo) {}

      upan::string title() const { return _title; }
      const upan::list<Menu::MenuEntryInfo>& menuEntryInfo() const { return _menuEntryInfo; }
    private:
      const upan::string _title;
      const upan::list<Menu::MenuEntryInfo> _menuEntryInfo;
    };

    void initMenuBar(const upan::list<MenuInfo>& menuInfo);
    int menuBarHeight() const { return _menuBarHeight; }
    void onMenuClick(Menu&);
    void onMenuHover(Menu&);
    void closeActiveMenu();
    void drawActiveMenu();
    bool isModelActive() const;
    void handleMouseEvent(const MouseEvent&, const UIObject&);

  private:
    UIRoot(int x, int y, int width, int height);

  protected:
    int drawX() const override;
    int drawY() const override;
    void draw() override;
    void drawTopDown() override;
    void drawToTop() override;

    bool captureMouseEvents() const override {
      return true;
    }

    bool isRectangularShape() override { return true; }
    IntersectInfo intersect(int x, int y) const override;

    void notifyChange(const ChangeState changeState) override;

    void updateViewport();
    int resizeLeft(int dx, bool isPrimary) override;
    int resizeRight(int dx, bool isPrimary) override;
    int resizeTop(int dy, bool isPrimary) override;
    int resizeBottom(int dy, bool isPrimary) override;
    bool isVisible() const override;

  private:
    Layout& layout() override {
      return _layout;
    }

  private:
    RectangularLayout _layout;
    bool _menuInitialized;
    int _menuBarHeight;
    Menu* _activeMenu;

    friend class GraphicsContext;
    friend class UIObjectManager;
  };
}
