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

#include <TextArea.h>

namespace upanui {
  class TerminalCommandExecutor;

  class Terminal : public TextArea {
  public:
    void setPrompt(const upan::string&);
    void insertCommand(const upan::string& str);
    void insertCommandOutput(const upan::string& str);
    void insertCommandOutput(const upan::vector<Character>& characters);

  private:
    Terminal(int x, int y, int width, int height, int leftMargin,
             const upan::string& prompt,
             TerminalCommandExecutor& terminalCommandExecutor,
             HorizontalPlacementType horizontalPlacementType,
             VerticalPlacementType verticalPlacementType);
    ~Terminal() {}

    void initialize();

    void enter() override;
    void moveup() override;
    void moveleft() override;
    void movehome() override;
    void moveend() override;
    void backspace() override;
    void cutSelection() override;
    void moveCursor(bool shiftPressed, bool mouseHeld, int x, int y) override;
    void editSelection() override;

  private:
    bool isPrimaryCommandLine();
    upan::string getCommandLine();

  private:
    upan::string _prompt;
    UIPosition _mouseSelectionCharacterPos;
    UIPosition _mouseSelectionCursorPos;
    TerminalCommandExecutor& _terminalCommandExecutor;
    friend class UIObjectFactory;
  };
}