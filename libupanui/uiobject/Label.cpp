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

namespace upanui {
  Label::Label(int x, int y,
               int width, int height,
               const upan::string& str,
               uint32_t fgColor,
               usfn::PreloadedFonts fontType,
               int fontFamily, int fontStyle, int fontSize,
               HorizontalTextAlignment hTextAlignment, VerticalTextAlignment vTextAlignment,
               HorizontalPlacementType horizontalPlacementType, VerticalPlacementType verticalPlacementType)
    : RectangleCanvas(x, y, width, height, horizontalPlacementType, verticalPlacementType),
      _c(nullptr), _str(str), _fontType(-1), _fontFamily(-1),
      _fontStyle(fontStyle), _fontSize(fontSize), _updateText(false),
      _hTextAlignment(hTextAlignment), _vTextAlignment(vTextAlignment), _textX(0), _textY(0) {
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

  void Label::alignText() {
    const int pw = width() - 2 * borderThickness();
    const int ph = height() - 2 * borderThickness();

    const int tw = (_fontSize / 2) * _str.length();//_textBuffer.w;
    const int th = _fontSize;

    const int hd = pw < tw ? 0 : (pw - tw);
    switch (_hTextAlignment) {
      case HorizontalTextAlignment::RIGHT:
        _textX = hd;
        break;
      case HorizontalTextAlignment::HCENTER:
        _textX = hd / 2;
        break;

      default:
        _textX = 0;
        break;
    }

    const int vd = ph < th ? 0 : (ph - th);
    switch (_vTextAlignment) {
      case VerticalTextAlignment::BOTTOM:
        _textY = vd;
        break;
      case VerticalTextAlignment::VCENTER:
        _textY = vd / 2;
        break;

      default:
        _textY = 0;
        break;
    }
  }

  void Label::updateText() {
    if (_updateText.get()) {
      upan::mutex_guard g(_updateMutex);
      free(_textBuffer.ptr);
      _textBuffer.ptr = nullptr;
      _textBuffer.bg = _textBuffer.fg;
      _c->DrawText(_str.c_str(), _textBuffer);
      alignText();
      _updateText.set(false);
    }
  }

  void Label::doDraw() {
    RectangleCanvas::doDraw();
    updateText();
    auto& drawBuf = drawBuffer();
    drawBuf.copy(_textX, _textY, (uint32_t *)_textBuffer.ptr, _textBuffer.w, _textBuffer.w, _textBuffer.h, false);
  }
}