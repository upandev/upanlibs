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
  class Terminal : public TextArea {
  public:
    void setPrompt(const upan::string&);

    class CommandExecutor {
    public:
      CommandExecutor() = default;
      CommandExecutor(const CommandExecutor&) = delete;
      CommandExecutor& operator=(const CommandExecutor&) = delete;

      virtual void execute(const upan::string& cmdLine) = 0;
    };

  private:
    Terminal(int x, int y, int width, int height, int leftMargin,
             const upan::string& prompt,
             CommandExecutor& commandExecutor,
             HorizontalPlacementType horizontalPlacementType,
             VerticalPlacementType verticalPlacementType);
    ~Terminal() override;

    void initialize();

    void onKeyboardEvent(const KeyboardEvent& event) override;
    void moveup() override;
    void moveleft() override;
    void movehome() override;
    void moveend() override;
    void backspace() override;
    void cutSelection() override;
    void moveCursor(bool shiftPressed, bool mouseHeld, int x, int y) override;
    void editSelection() override;
    void displayCommandLine();

    class TerminalOutputHandler : public upan::thread {
    public:
      explicit TerminalOutputHandler(Terminal& terminal);

    private:
      void run() override;
      Terminal& _terminal;
    };

  private:
    bool isPrimaryCommandLine();
    int terminalMasterFD() const { return _terminalMasterFD; }

  private:
    int _terminalMasterFD;
    int _terminalSlaveFD;
    upan::string _prompt;
    UIPosition _mouseSelectionCharacterPos;
    UIPosition _mouseSelectionCursorPos;
    CommandExecutor& _commandExecutor;
    TerminalOutputHandler _terminalOutputHandler;
    upan::string _commandLine;

    friend class UIObjectFactory;
  };
}