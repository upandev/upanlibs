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

#include <Terminal.h>
#include <fcntl.h>
#include <fs.h>
#include <KeyboardEvent.h>

namespace upanui {
  Terminal::Terminal(int x, int y, int width, int height,
                     int leftMargin, const upan::string& prompt,
                     CommandExecutor& commandExecutor,
                     HorizontalPlacementType horizontalPlacementType,
                     VerticalPlacementType verticalPlacementType) : TextArea(x, y, width, height, leftMargin, horizontalPlacementType, verticalPlacementType),
                     _terminalMasterFD(-1), _terminalSlaveFD(-1), _commandExecutor(commandExecutor), _terminalOutputHandler(*this) {
    setPrompt(prompt);
  }

  Terminal::~Terminal() {
  }

  void Terminal::initialize() {
    TextArea::init();

    _terminalMasterFD = posix_openpt(O_RDWR);
    _terminalSlaveFD = open(ptsname(_terminalMasterFD), O_RDWR);
    dup2(_terminalSlaveFD, STDIN_FD);
    dup2(_terminalSlaveFD, STDOUT_FD);
    dup2(_terminalSlaveFD, STDERR_FD);

    displayCommandLine();
    _terminalOutputHandler.start();
  }

  void Terminal::setPrompt(const upan::string& prompt) {
    auto fullPrompt = prompt + "> ";
    _prompt = fullPrompt;
  }

  bool Terminal::isPrimaryCommandLine() {
    const int lineIndex = characterPos().y();
    if (lineIndex == 0) {
      return true;
    }
    return !lines().get(lineIndex - 1).wrapped();
  }

  void Terminal::displayCommandLine() {
    printf("%s", _prompt.c_str());
  }

  void Terminal::onKeyboardEvent(const KeyboardEvent& event) {
    const auto ch = event.getData().getRch();
    if (isNewLine(ch)) {
      const upan::string& cmdLine = getCommandLine();
      putc('\n', stdout);
      _commandExecutor.execute(cmdLine);
      putc('\n', stdout);
      displayCommandLine();
    } else {
      putc(ch, stdout);
    }
  }

  upan::string Terminal::getCommandLine() {
    movehome();
    upan::string cmdLine;
    for(int i = characterPos().y(); i < lines().size(); ++i) {
      cmdLine += lines().get(i).toString(i == characterPos().y() ? _prompt.length() : 0);
    }
    moveend();
    return cmdLine;
  }

  void Terminal::moveup() {
  }

  void Terminal::moveleft() {
    if (isPrimaryCommandLine() && characterPos().x() == _prompt.length()) {
      return;
    }
    if (characterPos().x() == 0) {
      TextArea::moveup();
      TextArea::moveend();
    } else {
      TextArea::moveleft();
    }
  }

  void Terminal::movehome() {
    while(!isPrimaryCommandLine()) {
      TextArea::moveup();
    }
    TextArea::movehome();
    for(int i = 0; i < _prompt.length(); ++i) {
      moveright();
    }
  }

  void Terminal::moveend() {
    while ((characterPos().y() + 1) < lines().size()) {
      movedown();
    }
    TextArea::moveend();
  }

  void Terminal::backspace() {
    if (isPrimaryCommandLine() && characterPos().x() == _prompt.length()) {
      return;
    }
    TextArea::backspace();
  }

  void Terminal::cutSelection() {
    copySelection();
  }

  void Terminal::moveCursor(bool shiftPressed, bool mouseHeld, int x, int y) {
    upan::mutex_guard g(drawMutex());

    const auto& info = getLineCursorInfo(x, y);

    const UIPosition prevCharPos = _mouseSelectionCharacterPos;
    _mouseSelectionCharacterPos = info._charPos;
    _mouseSelectionCursorPos = info._curPos;

    scrollToY(_mouseSelectionCursorPos.y(), _mouseSelectionCharacterPos.y());

    updateSelectedArea( mouseHeld, true, prevCharPos, _mouseSelectionCharacterPos);

    notifyChange(ChangeState::Content);
  }

  void Terminal::editSelection() {
    unselectArea();
  }

  Terminal::TerminalOutputHandler::TerminalOutputHandler(upanui::Terminal& terminal) : _terminal(terminal) {}

  void Terminal::TerminalOutputHandler::run() {
    io_descriptor waitFDs[2];
    waitFDs[0]._fd = _terminal.terminalMasterFD();
    waitFDs[0]._ioType = IO_OP_TYPES::IO_Read;

    waitFDs[1]._fd = -1;

    io_descriptor readyFDs[2];
    readyFDs[0]._fd = -1;

    const int MAX_BUFFER_SIZE = 1024;
    char buffer[MAX_BUFFER_SIZE];

    try {
      while (true) {
        select(waitFDs, readyFDs);

        for(int i = 0; readyFDs[i]._fd >= 0; ++i) {
          if (readyFDs[i]._fd == _terminal.terminalMasterFD()) {
            int n = read(readyFDs[i]._fd, buffer, MAX_BUFFER_SIZE);
            for (int j = 0; j < n; ++j) {
              _terminal.handleInput(buffer[j], false);
            }
          }
        }
      }
    } catch(upan::exception& e) {
      e.Print();
    }
  }
}