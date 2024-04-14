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
#include <MouseEventHandler.h>
#include <map.h>
#include <vector.h>
#include <mutex.h>
#include <timer_thread.h>
#include <uniq_ptr.h>

namespace upanui {

  class TextArea : public RectangleCanvas {
  public:
    void moveup();
    void movedown();
    void moveleft();
    void moveright();
    void movehome();
    void moveend();
    void insert(uint8_t ch);
    void removech();
    void backspace();
    void selectAll();
    void cutSelection();
    void copySelection();
    void paste();

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
    static constexpr int DEFAULT_FONT_SIZE = 16;
    static constexpr int DEFAULT_SIDE_MARGIN = 8;
    static constexpr uint32_t DEFAULT_BG_COLOR = 0xFFFFFF;
    static constexpr uint32_t DEFAULT_FG_COLOR = 0;

    TextArea(int x, int y, int width, int32_t height);
    ~TextArea();

    void doDraw() override;

    int scrollY() const override { return _scrollY; }
    int updateScrollY(int sy);

    int scrollHeight() const override { return _scrollHeight; }
    void changeScrollHeight(int delta);

    void vscroll(int rows, int scrollableHeight) override;

    void onKeyboardEvent(const KeyboardEvent& event) override;

    void clearArea(int x, int  y, int32_t width, int32_t height);

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
        auto w = _fontSize / 2;
        const auto e = _fontSize / DEFAULT_FONT_SIZE;

        if (_style & usfn::STYLE_BOLD) {
          w += e;
          if (_style & usfn::STYLE_ITALIC) {
            w += e * 2;
          }
        } else if (_style & usfn::STYLE_ITALIC) {
          w += e;
        }
        return w;
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

    typedef upan::vector<Character> Characters;

    class Line {
    public:
      Line(uint8_t defaultHeight) : _width(MIN_CURSOR_WIDTH_BUFFER), _maxChHeight(defaultHeight), _wrapped(false) {}
      void insert(int pos, const Character &ch);
      void remove(int from, int last);

      int width() const { return _width; }
      uint8_t lineHeight() const { return _maxChHeight + DEFAULT_LINE_SPACE; }
      bool wrapped() const { return _wrapped; }
      void wrapped(bool wrapped) {
        _wrapped = wrapped;
      }
      int size() const { return _characters.size(); }
      const Characters& characters() const { return _characters; }
      const Character& characters(int i) const { return _characters[i]; }
    private:
      const int MIN_CURSOR_WIDTH_BUFFER = 8;
      const int DEFAULT_LINE_SPACE = 4;
      Characters _characters;
      int _width;
      uint8_t _maxChHeight;
      bool _wrapped;
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

      bool operator==(const Position& p) const {
        return _x == p._x && _y == p._y;
      }

      bool operator<(const Position& p) const {
        return _y == p._y ? _x < p._x : _y < p._y;
      }

      bool operator<=(const Position& p) const {
        return (*this < p) || (*this == p);
      }

      bool operator>(const Position& p) const {
        return !(*this <= p);
      }

      bool operator>=(const Position& p) const {
        return !(*this < p);
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

    typedef struct {
      int _lineIndex;
      int _lineTopY;
      int _lineBaseY;
    } VirtualYInfo;

    void handleMouseEvent(upanui::UIObject& sender, const upanui::MouseEvent& event);

    class TextAreaMouseHandler : public MouseEventHandler {
    public:
      explicit TextAreaMouseHandler(TextArea &parent) : _parent(parent) {}
      void onEvent(upanui::UIObject &uiObject, const upanui::MouseEvent &event) override {
        _parent.handleMouseEvent(uiObject, event);
      }
    private:
      TextArea& _parent;
    };

    class SelectedArea {
    public:
      SelectedArea() : _present(false) {}

      bool isPresent() const { return _present; }
      void setPresent(bool present) { _present = present; }

      Position pivot() const { return _pivot; }
      Position p1() const { return _p1; }
      Position p2() const { return _p2; }

      void setPivot(const Position& pivot) { _pivot = pivot; }
      void setRange(const Position& pa, const Position& pb);
      bool inRange(int x, int y) const;

    private:
      Position _pivot;
      Position _p1;
      Position _p2;
      bool _present;
    };

    class ScrollerChanges {
    public:
      ScrollerChanges() : _calibrate(false), _adjustScrollY(false), _scrollY(0) {}
      void capture(bool calibrate, bool adjustScrollY, int scrollY);
      void apply(VerticalScroller&);

    private:
      bool _calibrate;
      bool _adjustScrollY;
      int _scrollY;
    };

  private:
    void init();
    void processKeyboardEvent(const KeyboardEvent& event);
    void insert(const Character& ch);
    void enter();
    usfn::Context& getUSFNContext(usfn::PreloadedFonts fontType, uint8_t fontSize, uint16_t fontStyle);
    void scrollToCursor();
    void validateCursorPos() const;
    void updateCursorPosition(int charPosX, int charPosY, int cursorPosX, int cursorPosY);
    void updateCursor(bool showCursor);
    void moveCursor(bool isSelectionOn, int x, int y);
    uint32_t getChBgColor(int cx, int cy, const Character& ch) const;
    void fillCharacterBG(int x, int y, int cx, int cy, int height, const Character &ch);

    void insert(TextArea::Line& line, int lineX, int lineY, const TextArea::Characters& characters);
    void wrapremovech(int x, int y, int& deletedLine);
    void lineremovech(const int y);
    void renderLine(const Line &line, int charX, int charY, int baseDrawY);

    int getLineBaseY(int lineIndex);
    int getLineBaseX(int charX, int lineIndex);
    VirtualYInfo getLineAtVirtualY(int baseY, int rows);
    void renderLineTopDown(const VirtualYInfo& info);
    void renderLineBottomUp(const VirtualYInfo& info);
    void renderLineRange(const Position& p1, const Position& p2);

    bool isSelectKey(uint8_t) const;
    bool isTextModifyKey(uint8_t ch) const;
    bool isInsertableKey(uint16_t ch) const;
    void deleteSelectedArea();
    void unselectArea();
    void updateSelectedArea(bool isSelectionOn, bool isSelectKey, const Position &prevCharPos);

  private:
    static const uint8_t MAX_FONT_SIZE = 128;

    upan::vector<Line*> _lines;

    typedef upan::map<uint64_t, usfn::Context*> FontContextMap;
    FontContextMap _fontContexts;

    int _scrollY;
    int _scrollHeight;
    uint8_t _currentFontSize;
    uint8_t _currentFontType;
    uint16_t _currentStyle;
    uint32_t _currentFGColor;
    uint32_t _currentBGColor;
    int _maxLineCharWidth;
    Position _characterPos;
    Position _cursorPos;
    DrawBuffer _textBuffer;
    upan::mutex _drawMutex;
    CursorBlink _cursorBlinkThread;
    upan::uniq_ptr<TextAreaMouseHandler> _mouseHandler;
    SelectedArea _selectedArea;
    ScrollerChanges _scrollerChanges;
    Characters _copyBuffer;

    friend class UIObjectFactory;
  };
}