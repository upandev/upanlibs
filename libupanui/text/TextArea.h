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
#include <mutex.h>
#include <timer_thread.h>

namespace upanui {

  class TextArea : public RectangleCanvas {
  public:
    void enter();
    void moveup();
    void movedown();
    void moveleft();
    void moveright();
    void insert(uint16_t ch);
    void removech();
    void backspace();

    uint8_t getCurrentFontSize() const {
      return _currentFontSize;
    }

    void setCurrentFontSize(uint8_t currentFontSize) {
      _currentFontSize = currentFontSize;
    }

    usfn::PreloadedFonts getCurrentFontType() const {
      return (usfn::PreloadedFonts) _currentFontType;
    }

    void setCurrentFontType(usfn::PreloadedFonts currentFontType) {
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
    void doDraw() override;
    void onKeyboardEvent(const KeyboardEvent& event) override;

    void clearArea(int x, int  y, uint32_t width, uint32_t height);

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
      Line(uint8_t defaultHeight) : _width(0), _maxHeight(defaultHeight) {}
      void insert(int pos, Character& ch);
      void remove(int from, int last);

      uint32_t width() const { return _width; }
      uint8_t maxHeight() const { return _maxHeight; }
      void setMaxHeight(uint8_t maxHeight) {
        _maxHeight = maxHeight;
      }
      uint32_t size() const { return _characters.size(); }
      const upan::vector<Character*>& characters() const { return _characters; }
    private:
      upan::vector<Character*> _characters;
      uint32_t _width;
      uint8_t _maxHeight;
    };

    class Position {
    public:
      Position() : _x(0), _y(0) {}

      int x() const {
        return _x;
      }

      int y() const {
        return _y;
      }

      void set(int x, int y) {
        _x = x;
        _y = y;
      }

    private:
      int _x;
      int _y;
    };

    class CursorBlink : public upan::timer_thread {
    public:
      explicit CursorBlink(TextArea& textArea);
      void on_timer_trigger() override;
      TextArea& _textArea;
      bool _showCursorToggle;
    };

    usfn::Context& getUSFNContext(usfn::PreloadedFonts fontType);
    void validateCursorPos() const;
    void updateCursor(int x, int y);
    void updateCursor(bool showCursor);
    void RenderLine(const Line& line, int charX, int baseDrawY);

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
    Position _characterPos;
    Position _cursorPos;
    DrawBuffer _textBuffer;
    upan::mutex _drawMutex;
    CursorBlink _cursorBlinkThread;

    friend class UIObjectFactory;
  };
}