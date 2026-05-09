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
#include <sys/select.h>
#include <KeyboardEvent.h>
#include <GraphicsContext.h>

namespace upanui {
  Terminal::Terminal(int x, int y, int width, int height,
                     int leftMargin, const upan::string& prompt,
                     CommandExecutor& commandExecutor,
                     HorizontalPlacementType horizontalPlacementType,
                     VerticalPlacementType verticalPlacementType) : TextArea(x, y, width, height, leftMargin, horizontalPlacementType, verticalPlacementType),
                     _terminalMasterFD(-1), _terminalSlaveFD(-1), _commandExecutor(commandExecutor),
                     _terminalInputHandler(*this), _terminalOutputHandler(*this) {
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
    _terminalInputHandler.start();
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
    printf("\n%s", _prompt.c_str());
  }

  void Terminal::handleKeyboardInput(const uint8_t ch) {
    if (ch == Keyboard_BACKSPACE) {
      if (!_commandLine.empty()) {
        _commandLine.pop_back();
        putchar(Keyboard_BACKSPACE);
      }
    } else if (isNewLine(ch)) {
      _commandExecutor.execute(_commandLine);
      _commandLine.clear();
      displayCommandLine();
    } else if (isInsertableKey(ch)) {
      _commandLine += ch;
    }
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
    while (!isPrimaryCommandLine()) {
      TextArea::moveup();
    }
    TextArea::movehome();
    for (int i = 0; i < _prompt.length(); ++i) {
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

    updateSelectedArea(mouseHeld, true, prevCharPos, _mouseSelectionCharacterPos);

    notifyChange(ChangeState::Content);
  }

  void Terminal::editSelection() {
    unselectArea();
  }

  void Terminal::processInput(int fd, bool isOut) {
    fd_set readfds;
    const int nfds = fd + 1;

    const int MAX_BUFFER_SIZE = 1024;
    uint8_t buffer[MAX_BUFFER_SIZE];

    try {
      while (true) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        if (select(nfds, &readfds, NULL, NULL, NULL) >= 1) {
          if (FD_ISSET(fd, &readfds)) {
            int n = read(fd, buffer, MAX_BUFFER_SIZE);
            for (int j = 0; j < n; ++j) {
              if (isOut) {
                handleInput(buffer[j], false);
              } else {
                handleKeyboardInput(buffer[j]);
              }
            }
          }
        }
      }
    } catch (upan::exception& e) {
      e.Print();
    }
  }
}