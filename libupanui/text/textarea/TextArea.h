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
#include <UIPosition.h>
#include <Character.h>
#include <TextLine.h>
#include <usfncontexts.h>
#include <TextBuffer.h>

namespace upanui {

  class TextArea : public RectangleCanvas {
  public:
    void moveup();
    void movedown();
    void moveleft();
    void moveright();
    void movehome();
    void moveend();
    void pageup();
    void pagedown();
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

  private:

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

      UIPosition pivot() const { return _pivot; }
      UIPosition p1() const { return _p1; }
      UIPosition p2() const { return _p2; }

      void setPivot(const UIPosition& pivot) { _pivot = pivot; }
      void setRange(const UIPosition& pa, const UIPosition& pb);
      bool inRange(int x, int y) const;

    private:
      UIPosition _pivot;
      UIPosition _p1;
      UIPosition _p2;
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
    void scrollToCursor();
    void validateCursorPos() const;
    void updateCursorPosition(int charPosX, int charPosY, int cursorPosX, int cursorPosY);
    void updateCursor(bool showCursor);
    void moveCursor(bool isSelectionOn, int x, int y);
    uint32_t getChBgColor(int cx, int cy, const Character& ch) const;

    void insert(TextLine& line, int lineX, int lineY, const Characters& characters);
    void wrapremovech(int x, int y, int& deletedLine);
    void lineremovech(const int y);
    void renderLine(const TextLine &line, int charX, int charY, int baseDrawY);

    int getLineBaseY(int lineIndex);
    int getLineBaseX(int charX, int lineIndex);
    VirtualYInfo getLineAtVirtualY(int baseY, int rows);
    void renderLineTopDown(const VirtualYInfo& info);
    void renderLineBottomUp(const VirtualYInfo& info);
    void renderLineRange(const UIPosition& p1, const UIPosition& p2);

    bool isSelectKey(uint8_t) const;
    bool isTextModifyKey(uint8_t ch) const;
    bool isInsertableKey(uint16_t ch) const;
    void deleteSelectedArea();
    void unselectArea();
    void updateSelectedArea(bool isSelectionOn, bool isSelectKey, const UIPosition &prevCharPos);

  private:
    int _scrollY;
    int _scrollHeight;
    uint8_t _currentFontSize;
    uint8_t _currentFontType;
    uint16_t _currentStyle;
    uint32_t _currentFGColor;
    uint32_t _currentBGColor;
    int _maxLineCharWidth;
    UIPosition _characterPos;
    UIPosition _cursorPos;
    upan::mutex _drawMutex;
    CursorBlink _cursorBlinkThread;
    upan::uniq_ptr<TextAreaMouseHandler> _mouseHandler;
    SelectedArea _selectedArea;
    ScrollerChanges _scrollerChanges;
    Characters _copyBuffer;
    upan::vector<TextLine*> _lines;
    usfn::Contexts _fontContexts;
    TextBuffer _textBuffer;

    friend class UIObjectFactory;
  };
}