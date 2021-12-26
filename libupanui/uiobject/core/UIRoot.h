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

#include <stdlib.h>
#include <UIObjectImpl.h>
#include <RectangularLayout.h>
#include <DrawBuffer.h>

namespace upanui {
  class MouseEventHandler;

  class UIRoot : public UIObjectImpl {
  private:
    UIRoot(const int x, const int y, const uint32_t width, const uint32_t height);

  public:
    void onDrag(MouseEventHandler& handler) {
      _onDragHandler = upan::option<MouseEventHandler&>(handler);
    }

  protected:
    void onMouseEvent(const MouseEvent& event) override;

    int drawX() const override;
    int drawY() const override;
    void draw() override;
    void drawTopDown() override;
    void drawToTop() override;

    void positionChanged() override;
    void sizeChanged() override;
    void contentChanged() override;

  private:
    Layout& layout() override {
      return _layout;
    }
    DrawBuffer& drawBuffer() override {
      return _drawBuffer;
    }

  private:
    DrawBuffer _drawBuffer;
    upan::option<MouseEventHandler&> _onDragHandler;
    RectangularLayout _layout;

    friend class GraphicsContext;
  };
}
