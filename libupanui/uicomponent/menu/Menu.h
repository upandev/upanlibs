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

#include <RectangleCanvas.h>
#include <map.h>

namespace upanui {
  class UIRoot;
  class Label;

  class Menu : public RectangleCanvas {
  public:
    static const int MENU_LABEL_FONT_SIZE = 16;
    void select(bool selected);
    void drawPanel();

    class ActionHandler {
    public:
      virtual ~ActionHandler() {}
      virtual void invoke(int id, const upan::string& name) = 0;
    };

    class MenuEntryInfo {
    public:
      MenuEntryInfo(int id, const upan::string& name, ActionHandler& handler) :
        _id(id), _name(name), _handler(handler) {
      }

      int id() const { return _id; }
      upan::string name() const { return _name; }
      ActionHandler& handler() const { return _handler; }

    private:
      const int _id;
      const upan::string _name;
      ActionHandler& _handler;
    };

  protected:
    virtual ~Menu() {}
    Menu(UIRoot& uiRoot, const upan::string& title, int x, int y, int height);
    void init(const upan::list<MenuEntryInfo>& menuEntryInfo);

    void onKeyboardEvent(const KeyboardEvent& event) override;
    void onMouseEvent(const MouseEvent& event) override;
    void onMouseFocus() override;

  private:
    UIRoot& _uiRoot;
    const upan::string _title;
    Label* _uiLabel;
    RectangleCanvas* _uiPanel;

    friend class UIObjectFactory;
  };
}