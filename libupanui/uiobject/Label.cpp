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

#include <Label.h>
#include <usfncontext.h>
#include <GCoreFunctions.h>
#include <TextWriter.h>

namespace upanui {
  Label::Label(const int x, const int y,
               const uint32_t width, const uint32_t height,
               const upan::string& str,
               uint32_t fgColor,
               usfn::PreloadedFonts fontType,
               int fontFamily, int fontStyle, int fontSize)
    : RectangleCanvas(x, y, width, height), _c(nullptr), _str(str),
      _fontStyle(fontStyle), _fontSize(fontSize), _updateText(false) {
    _textBuffer.ptr = nullptr;
    _textBuffer.fg = fgColor | GCoreFunctions::ALPHA_MASK;
    setFont(fontType, fontFamily);
  }

  void Label::setFont(usfn::PreloadedFonts fontType, int fontFamily) {
    if (_fontType != fontType || _fontFamily != fontFamily) {
      upan::mutex_guard g(_updateMutex);
      delete _c;
      _c = new usfn::Context();
      _c->Load(usfn::Context::GetPreloadedFont(fontType));
      _fontType = fontType;
      _fontFamily = fontFamily;
      _c->Select(_fontFamily, nullptr, _fontStyle, _fontSize);
      markForUpdateText();
    }
  }

  void Label::setFontStyle(int fontStyle) {
    if (_fontStyle != fontStyle) {
      upan::mutex_guard g(_updateMutex);
      _fontStyle = fontStyle;
      _c->Select(_fontFamily, nullptr, _fontStyle, _fontSize);
      markForUpdateText();
    }
  }

  void Label::setFontSize(int fontSize) {
    if (_fontSize != fontSize) {
      upan::mutex_guard g(_updateMutex);
      _fontSize = fontSize;
      _c->Select(_fontFamily, nullptr, _fontStyle, _fontSize);
      markForUpdateText();
    }
  }

  void Label::setText(const upan::string &str) {
    if (_str != str) {
      _str = str;
      markForUpdateText();
    }
  }

  void Label::setFGColor(uint32_t fgColor) {
    fgColor |= GCoreFunctions::ALPHA_MASK;
    if(_textBuffer.fg != fgColor) {
      _textBuffer.fg = fgColor;
      markForUpdateText();
    }
  }

  void Label::markForUpdateText() {
    _updateText.set(true);
    notifyChange(ChangeState::Content);
  }

  void Label::updateText() {
    if (_updateText.get()) {
      upan::mutex_guard g(_updateMutex);
      free(_textBuffer.ptr);
      _textBuffer.ptr = nullptr;
      _textBuffer.bg = _textBuffer.fg;
      _c->DrawText(_str.c_str(), _textBuffer);
      _updateText.set(false);
    }
  }

  void Label::doDraw() {
    RectangleCanvas::doDraw();
    updateText();
    auto& drawBuf = drawBuffer();
    drawBuf.copy((uint32_t *)_textBuffer.ptr, _textBuffer.w, _textBuffer.h, false);
  }
}