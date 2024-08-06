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
#include <kb.h>

namespace upanui {
  Terminal::Terminal(int x, int y, int width, int height,
                     int leftMargin, const upan::string& prompt,
                     CommandExecutor& commandExecutor,
                     HorizontalPlacementType horizontalPlacementType,
                     VerticalPlacementType verticalPlacementType) : TextArea(x, y, width, height, leftMargin, horizontalPlacementType, verticalPlacementType),
                     _commandExecutor(commandExecutor) {
    setPrompt(prompt);
  }

  void Terminal::initialize() {
    TextArea::init();
    insertCommand(_prompt);
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

  upan::string Terminal::getCommandLine() {
    movehome();
    upan::string cmdLine;
    for(int i = characterPos().y(); i < lines().size(); ++i) {
      cmdLine += lines().get(i).toString(i == characterPos().y() ? _prompt.length() : 0);
    }
    return cmdLine;
  }

  void Terminal::enter() {
    const upan::string& cmdLine = getCommandLine();
    moveend();
    TextArea::enter();
    _commandExecutor.execute(cmdLine);
    TextArea::enter();
    insertCommand(_prompt);
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

  void Terminal::insertCommand(const upan::string& str) {
    for (int i = 0; i < str.length(); ++i) {
      insert(str[i]);
    }
  }

  void Terminal::insertCommandOutput(const upan::string& str) {
    upan::vector<Character> characters;
    for (int i = 0; i < str.length(); ++i) {
      characters.push_back(createCharacter(str[i]));
    }
    insertCommandOutput(characters);
  }

  void Terminal::insertCommandOutput(const upan::vector<Character>& characters) {
    for(const auto& c : characters) {
      if (isNewLine(c.getCh())) {
        TextArea::enter();
      } else {
        doInsert(c);
      }
    }
  }
}