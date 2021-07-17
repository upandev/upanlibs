/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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

#include <UIObject.h>
#include <list.h>
#include <atomicop.h>

namespace upanui {
  class CanvasBuilder;
  class Canvas;

  class Frame : public UIObject {
  public:
    Frame(uint32_t* frameBuffer, uint32_t width, uint32_t height);
    ~Frame();

    void draw();
    void touch();
    void addCanvas(const CanvasBuilder& builder);

    uint32_t width() const override {
      return _width;
    }

    virtual uint32_t height() const override {
      return _height;
    }

    virtual const uint32_t* frameBuffer() const override {
      return _frameBuffer;
    }

  private:
    uint32_t _width;
    uint32_t _height;
    uint32_t* _frameBuffer;
    upan::atomic::integral<bool> _isDirty;
    upan::list<Canvas*> _canvasLayers;
  };
}