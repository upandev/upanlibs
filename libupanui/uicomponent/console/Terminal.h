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

#include <termios.h>
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

    void handleKeyboardInput(const uint8_t ch);
    void processInput(int fd, bool isOut);
    void moveup() override;
    void moveleft() override;
    void movehome() override;
    void moveend() override;
    void enter() override;
    void backspace() override;
    void cutSelection() override;
    void moveCursor(bool shiftPressed, bool mouseHeld, int x, int y) override;
    void editSelection() override;
    void displayCommandLine();

    class TerminalOutputHandler : public upan::thread {
    public:
      explicit TerminalOutputHandler(Terminal& terminal) : _terminal(terminal) {}

    private:
      void run() override {
        _terminal.processInput(_terminal.terminalMasterFD(), true);
      }
      Terminal& _terminal;
    };

    class TerminalInputHandler : public upan::thread {
    public:
      explicit TerminalInputHandler(Terminal& terminal) : _terminal(terminal) {}

    private:
      void run() override {
        _terminal.processInput(STDIN_FD, false);
      }
      Terminal& _terminal;
    };

  private:
    bool isPrimaryCommandLine();
    upan::string getCommandLine();
    int terminalMasterFD() const { return _terminalMasterFD; }

  private:
    int _terminalMasterFD;
    int _terminalSlaveFD;
    upan::string _prompt;
    UIPosition _mouseSelectionCharacterPos;
    UIPosition _mouseSelectionCursorPos;
    CommandExecutor& _commandExecutor;
    TerminalInputHandler _terminalInputHandler;
    TerminalOutputHandler _terminalOutputHandler;
    upan::string _commandLine;
    termios _termios;

    friend class UIObjectFactory;
  };
}