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
#include <map.h>
#include <vector.h>

namespace upanui {

  class TextArea : public RectangleCanvas {
  public:
    void insert(uint16_t ch);

    uint8_t getCurrentFontSize() const {
      return _currentFontSize;
    }

    void setCurrentFontSize(uint8_t currentFontSize) {
      _currentFontSize = currentFontSize;
    }

    uint8_t getCurrentFontType() const {
      return _currentFontType;
    }

    void setCurrentFontType(uint8_t currentFontType) {
      _currentFontType = currentFontType;
    }

    uint16_t getCurrentStyle() const {
      return _currentStyle;
    }

    void setCurrentStyle(uint16_t currentStyle) {
      _currentStyle = currentStyle;
    }

    uint32_t getCurrentFgColor() const {
      return _currentFGColor;
    }

    void setCurrentFgColor(uint32_t currentFgColor) {
      _currentFGColor = currentFgColor;
    }

    uint32_t getCurrentBgColor() const {
      return _currentBGColor;
    }

    void setCurrentBgColor(uint32_t currentBgColor) {
      _currentBGColor = currentBgColor;
    }

  private:
    TextArea(int x, int y, uint32_t width, uint32_t height);
    ~TextArea();

  private:
    class Character {
    public:
      Character(uint16_t ch, uint8_t fontSize, uint8_t fontType,
                uint16_t style, uint32_t fgColor, uint32_t bgColor) :
                _ch(ch), _fontSize(fontSize), _fontType(fontType),
                _style(style), _fgColor(fgColor), _bgColor(bgColor) {}

      uint16_t getCh() const {
        return _ch;
      }

      uint8_t getFontSize() const {
        return _fontSize;
      }

      uint8_t getChWidth() const {
        return _fontSize / 2;
      }

      uint8_t getChHeight() const {
        return _fontSize;
      }

      usfn::PreloadedFonts getFontType() const {
        return (usfn::PreloadedFonts)_fontType;
      }

      uint16_t getStyle() const {
        return _style;
      }

      uint32_t getFgColor() const {
        return _fgColor;
      }

      uint32_t getBgColor() const {
        return _bgColor;
      }

      void setCh(uint16_t ch) {
        _ch = ch;
      }

      void setFontSize(uint8_t fontSize) {
        _fontSize = fontSize;
      }

      void setFontType(usfn::PreloadedFonts fontType) {
        _fontType = fontType;
      }

      void setStyle(uint16_t style) {
        _style = style;
      }

      void setFgColor(uint32_t fgColor) {
        _fgColor = fgColor;
      }

      void setBgColor(uint32_t bgColor) {
        _bgColor = bgColor;
      }

    private:
      uint16_t _ch;
      uint8_t _padding1;
      uint8_t _fontSize;

      uint8_t _fontType;
      uint8_t _padding2;
      uint16_t _style;

      uint32_t _fgColor;
      uint32_t _bgColor;
    } PACKED;

    class Line {
    public:
      Line() : _width(0), _maxHeight(0) {}
      void insert(int pos, Character& ch);
      uint32_t width() const { return _width; }
      uint8_t maxHeight() const { return _maxHeight; }
      void setMaxHeight(uint8_t maxHeight) {
        _maxHeight = maxHeight;
      }
      uint32_t size() const { return _characters.size(); }
    private:
      upan::vector<Character*> _characters;
      uint32_t _width;
      uint8_t _maxHeight;
    };

    class CharacterPos {
    public:
      CharacterPos() : _row(0), _column(0) {}

      uint32_t getRow() const {
        return _row;
      }

      uint32_t getColumn() const {
        return _column;
      }

      void set(uint32_t row, uint32_t column) {
        _row = row;
        _column = column;
      }

    private:
      uint32_t _row;
      uint32_t _column;
    };

    usfn::Context& getUSFNContext(usfn::PreloadedFonts fontType);
    void validateCursorPos() const;
    void scrollOnEnter();

  private:
    static const uint8_t MAX_FONT_SIZE = 128;

    upan::vector<Line*> _lines;

    typedef upan::map<usfn::PreloadedFonts, usfn::Context*> FontContextMap;
    FontContextMap _fontContexts;

    uint8_t _currentFontSize;
    uint8_t _currentFontType;
    uint16_t _currentStyle;
    uint32_t _currentFGColor;
    uint32_t _currentBGColor;
    CharacterPos _characterPos;
    uint32_t _cursorPos;
    DrawBuffer _textBuffer;

    friend class UIObjectFactory;
  };
}