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

#include <Frame.h>
#include <GraphicsContext.h>
#include <CanvasBuilder.h>
#include <Canvas.h>

namespace upanui {
  Frame::Frame(uint32_t* frameBuffer, uint32_t width, uint32_t height)
  : _width(width), _height(height), _frameBuffer(frameBuffer), _isDirty(false) {
  }

  Frame::~Frame() noexcept {
    for(auto c : _canvasLayers) {
      delete c;
    }
  }

  void Frame::draw() {
    if (_isDirty.get()) {
      _isDirty.set(false);
      for(auto c : _canvasLayers) {
        memcpy(_frameBuffer, c->frameBuffer(), c->width() * c->height());
      }
    }
  }

  void Frame::touch() {
    _isDirty.set(true);
  }

  void Frame::addCanvas(const CanvasBuilder& builder) {
    _canvasLayers.push_back(&builder.create(*this));
  }
}