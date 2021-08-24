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

#include <GraphicsContext.h>
#include <Frame.h>

namespace upanui {
  GraphicsContext* GraphicsContext::_instance = nullptr;

  void GraphicsContext::Init() {
    if (_instance) {
      throw upan::exception(XLOC, "GraphicsContext is already created!");
    }
    _instance = new GraphicsContext();
  }

  void GraphicsContext::Destroy() {
    if (_instance) {
      delete _instance;
      _instance = nullptr;
    }
  }

  GraphicsContext& GraphicsContext::Instance() {
    if (!_instance) {
      throw upan::exception(XLOC, "GraphicsContext is not initialized yet!");
    }
    return *_instance;
  }

  GraphicsContext::GraphicsContext() : _frame(nullptr) {
    FrameBufferInfo frameBufferInfo;
    init_gui_frame(&frameBufferInfo);

    upanui::FrameBuffer frameBuffer(frameBufferInfo);
    upanui::Viewport viewport(0, 0, frameBufferInfo._width, frameBufferInfo._height);

    _frame.reset(new upanui::Frame(frameBuffer, viewport));
  }

  GraphicsContext::~GraphicsContext() {
  }
}