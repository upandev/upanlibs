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

#include <UIElement.h>

namespace upanui {
  class FrameBuffer;

  class Canvas : public UIElement {
  protected:
    virtual ~Canvas() {}
    Canvas(const int x, const int y, const uint32_t width, const uint32_t height);

  public:
    void backgroundColor(const uint32_t);
    uint32_t backgroundColor() const {
      return _bgColor;
    }
    void backgroundColorAlpha(const uint32_t);
    uint32_t  backgroundColorAlpha() const {
      return _bgAlpha;
    }

  protected:
    virtual void onBackgroundColorChange() {}
    virtual uint32_t backgroundColorForDraw() {
      return backgroundColor();
    }
    virtual void doDraw(const FrameBuffer& frameBuffer, const int baseX, const int baseY) {}
    void fill(const FrameBuffer& framebuffer, int x1, int y1, int x2, int y2, uint32_t color, uint32_t alpha);

  private:
    void draw() override;

  private:
    uint32_t _bgColor;
    uint32_t _bgAlpha;
  };
}