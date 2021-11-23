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

#include <GraphicsContext.h>
#include <Frame.h>
#include <RootCanvas.h>

namespace upanui {
  void GraphicsContext::Init() {
    interop::graphics_context::init();
  }

  void GraphicsContext::Destroy() {
    interop::graphics_context::destroy();
  }

  GraphicsContext& GraphicsContext::Instance() {
    return interop::graphics_context::instance();
  }

  GraphicsContext::GraphicsContext() : _frame(nullptr), _evenManager(nullptr) {
    FrameBufferInfo frameBufferInfo;
    init_gui_frame(&frameBufferInfo);

    upanui::FrameBuffer frameBuffer(frameBufferInfo);
    ViewportInfo viewportInfo;
    get_viewport(&viewportInfo);
    upanui::Viewport viewport(viewportInfo);

    _frame.reset(new upanui::Frame(frameBuffer, viewport));
  }

  GraphicsContext::~GraphicsContext() {
  }

  RootCanvas& GraphicsContext::initRootCanvas() {
    return initRootCanvas(_frame->viewport().x1(), _frame->viewport().y1(), _frame->viewport().width(), _frame->viewport().height(), true);
  }

  RootCanvas& GraphicsContext::initRootCanvasWithoutAutoRefresh() {
    return initRootCanvas(_frame->viewport().x1(), _frame->viewport().y1(), _frame->viewport().width(), _frame->viewport().height(), false);
  }

  RootCanvas& GraphicsContext::initRootCanvas(const int x, const int y, const uint32_t width, const uint32_t height, const bool autoRefresh) {
    if (_rootCanvas.get() != nullptr) {
      throw upan::exception(XLOC, "RootCanvas is already initialized!");
    }
    _rootCanvas.reset(new RootCanvas(x, y, width, height));
    _uiObjectManager.reset(new UIObjectManager(*_rootCanvas, autoRefresh));
    initEventManager();
    return *_rootCanvas;
  }

  EventManager& GraphicsContext::initEventManager() {
    if (_evenManager.get() != nullptr) {
      throw upan::exception(XLOC, "EventManager is already initialized!");
    }
    _evenManager.reset(new EventManager());
    return *_evenManager;
  }

  EventManager& GraphicsContext::eventManager() {
    if (_evenManager.get() == nullptr) {
      throw upan::exception(XLOC, "EventManager is not initialized!");
    }
    return *_evenManager;
  }

  upan::option<UIObject&> GraphicsContext::setFocus(UIObject& uiObject) {
    return _uiObjectManager->setFocus(uiObject);
  }

  UIObjectManager& GraphicsContext::uiObjectManager() {
    if (_uiObjectManager.get() == nullptr) {
      throw upan::exception(XLOC, "UIObjectManager is not initialized!");
    }
    return *_uiObjectManager;
  }
}