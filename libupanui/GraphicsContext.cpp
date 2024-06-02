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
#include "GCoreFunctions.h"

namespace upanui {
  void GraphicsContext::Init() {
    interop::graphics_context::init();
  }

  void GraphicsContext::Destroy(GraphicsContext* gc) {
    interop::graphics_context::destroy(gc);
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
    _frame->fillRect(0, 0, _frame->viewport().width(), _frame->viewport().height(), 0 /* default 100% transparency */);
  }

  GraphicsContext::~GraphicsContext() {
  }

  UIRoot& GraphicsContext::initUIRoot() {
    return initUIRoot(_frame->viewport().x1(), _frame->viewport().y1(), _frame->viewport().width(), _frame->viewport().height(), true);
  }

  UIRoot& GraphicsContext::initUIRootWithoutAutoRefresh() {
    return initUIRoot(_frame->viewport().x1(), _frame->viewport().y1(), _frame->viewport().width(), _frame->viewport().height(), false);
  }

  UIRoot& GraphicsContext::initUIRoot(int x, int y, int width, int height, const bool autoRefresh) {
    if (_uiObjectManager.get() != nullptr) {
      throw upan::exception(XLOC, "UIRoot is already initialized!");
    }
    UIRoot& rootCanvas = *new UIRoot(x, y, width, height);
    _uiObjectManager.reset(new UIObjectManager(rootCanvas, autoRefresh));
    initEventManager();
    return rootCanvas;
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

  void GraphicsContext::setGuiBase(bool isGuiBase) {
    set_gui_base(isGuiBase);
  }

  UIObjectManager& GraphicsContext::uiObjectManager() {
    if (_uiObjectManager.get() == nullptr) {
      throw upan::exception(XLOC, "UIObjectManager is not initialized!");
    }
    return *_uiObjectManager;
  }

  void GraphicsContext::setMouseCursorType(MouseCursorType type) {
    if (_mouseCursorType != type) {
      _mouseCursorType = type;
      set_mouse_cursor_type(_mouseCursorType);
    }
  }

  GraphicsContext::Transaction::Transaction() {
    GraphicsContext::Instance()._uiObjectManager->_autoRefreshHandler.pause();
    GraphicsContext::Instance()._uiObjectManager->drawLock().lock();
  }

  GraphicsContext::Transaction::~Transaction() {
    GraphicsContext::Instance()._uiObjectManager->drawLock().unlock();
    GraphicsContext::Instance()._uiObjectManager->_autoRefreshHandler.start();
  }
}