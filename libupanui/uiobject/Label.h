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
#include <atomicop.h>
#include <mutex.h>

namespace upanui {
  class Label : public RectangleCanvas {
  public:
    void setText(const upan::string& str);
    void setFGColor(uint32_t fgColor);
    void setFont(usfn::PreloadedFonts fontType, int fontFamily);
    void setFontStyle(int fontStyle);
    void setFontSize(int fontSize);

    upan::string getText() const {
      return _str;
    }
    uint32_t getFGColor() const {
      return _textBuffer.fg;
    }

  protected:
    virtual ~Label() {}
    Label(int x, int y,
          int width, int height,
          const upan::string& str,
          uint32_t fgColor,
          usfn::PreloadedFonts fontType,
          int fontFamily,
          int fontStyle,
          int fontSize,
          HorizontalPlacementType horizontalPlacementType,
          VerticalPlacementType verticalPlacementType);
    void doDraw() override;

  private:
    void markForUpdateText();
    void updateText();

    usfn::Context* _c;
    upan::string _str;
    int _fontType;
    int _fontFamily;
    int _fontStyle;
    int _fontSize;
    usfn::FrameBuffer _textBuffer;
    upan::atomic::integral<bool> _updateText;
    upan::mutex _updateMutex;
    friend class UIObjectFactory;
  };
}