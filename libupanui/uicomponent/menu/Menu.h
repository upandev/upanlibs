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

namespace upanui {
  class UIRoot;
  class Label;

  class Menu : public RectangleCanvas {
  public:
    static const int MENU_LABEL_FONT_SIZE = 16;
    void select(bool selected);
    void drawPanel();

  protected:
    virtual ~Menu() {}
    Menu(UIRoot& uiRoot, const upan::string& title, int x, int y, int height);
    void init();

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