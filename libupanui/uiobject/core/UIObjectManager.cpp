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

#include <UIObjectManager.h>
#include <UIObject.h>
#include <UIRoot.h>
#include <GraphicsContext.h>
#include <typeinfo.h>

namespace upanui {
  UIObjectManager::UIObjectManager(UIRoot& rootCanvas, const bool autoRefresh)
    : _rootCanvas(rootCanvas),
      _focusedUIObject(rootCanvas),
      _mouseFocusedObject(upan::option<UIObject&>::empty()),
      _autoRefreshHandler(*this), _hasAlpha(false), _recalcHasAlpha(true) {
    _proxyParent.reset(new UIProxyParent());
    add(*_proxyParent, rootCanvas);
    _parentChildMap.insert(ParentChildMap::value_type(&_rootCanvas, upan::list<UIObject*>()));
    if (autoRefresh) {
      _autoRefreshHandler.start();
      queueForRedraw(_rootCanvas);
    }
  }

  UIObjectManager::~UIObjectManager() {
    _autoRefreshHandler.stop();
    destroy(_rootCanvas);
  }

  UIObject& UIObjectManager::parent(const UIObject& child) const {
    upan::mutex_guard g(_uiObjectTreeMutex);

    auto i = _childParentMap.find(&child);
    if (i == _childParentMap.end()) {
      throw upan::exception(XLOC, "there is no parent UIObject for this child");
    }
    return *i->second;
  }

  const upan::list<UIObject*>& UIObjectManager::children(UIObject& parent) {
    upan::mutex_guard g(_uiObjectTreeMutex);
    return _parentChildMap[&parent];
  }

  void UIObjectManager::add(UIObject& parent, UIObject& child) {
    upan::mutex_guard g(_uiObjectTreeMutex);

    if (&parent == &child) {
      throw upan::exception(XLOC, "parent and child can't be same");
    }

    auto i = _childParentMap.find(&child);
    if (i != _childParentMap.end()) {
      throw upan::exception(XLOC, "UIObject is already a child of another UIObject");
    }
    _parentChildMap[&parent].push_back(&child);
    _childParentMap[&child] = &parent;
  }

  void UIObjectManager::remove(UIObject& child) {
    upan::mutex_guard g(_uiObjectTreeMutex);
    _parentChildMap[&parent(child)].erase(&child);
  }

  void UIObjectManager::destroy(UIObject& uiObject) {
    upan::mutex_guard g(_uiObjectTreeMutex);

    remove(uiObject);
    _childParentMap.erase(&uiObject);

    _parentChildMap[&uiObject].foreach([this](UIObject* child) { destroy(*child); });
    _parentChildMap.erase(&uiObject);

    upan::mutex_guard g1(_uiObjectQueueMutex);
    _modifiedUIObjects.erase(&uiObject);

    if (!_focusedUIObject.isEmpty()) {
      if (&_focusedUIObject.value() == &uiObject) {
        _focusedUIObject = upan::option<UIObject&>::empty();
      }
    }

    if (!_mouseFocusedObject.isEmpty()) {
      if (&_mouseFocusedObject.value() == &uiObject) {
        _mouseFocusedObject = upan::option<UIObject&>::empty();
      }
    }

    delete &uiObject;
  }

  void UIObjectManager::queueForRedraw(UIObject& uiObject) {
    upan::mutex_guard g(_uiObjectQueueMutex);

    if (_modifiedUIObjects.size() == MAX_OBJECTS_UPDATE_QUEUE || (&_rootCanvas == &uiObject)) {
      _modifiedUIObjects.clear();
      _modifiedUIObjects.push_back(&_rootCanvas);
    } else if (_modifiedUIObjects.empty() || _modifiedUIObjects.back() != &uiObject) {
      _modifiedUIObjects.push_back(&uiObject);
    }
  }

  void UIObjectManager::draw() {
    upan::mutex_guard g(_uiObjectTreeMutex);

    UIObject* modifiedUIObjects[MAX_OBJECTS_UPDATE_QUEUE];
    int count = 0;

    {
      upan::mutex_guard g1(_uiObjectQueueMutex);
      for(auto it = _modifiedUIObjects.begin(); it != _modifiedUIObjects.end(); ++it) {
        modifiedUIObjects[count++] = *it;
      }
      _modifiedUIObjects.clear();
    }

    for(int i = 0; i < count; ++i) {
      modifiedUIObjects[i]->draw();
    }
    if (count > 0) {
      if (_recalcHasAlpha) {
        GraphicsContext::Instance().frame().hasAlpha(_rootCanvas.hasAlpha());
        _recalcHasAlpha = false;
      }
      _rootCanvas.updateViewport();
      GraphicsContext::Instance().frame().touch();
    }
  }

  UIObjectManager::AutoRefreshHandler::AutoRefreshHandler(UIObjectManager& uiObjectManager) : upan::timer_thread(50), _uiObjectManager(uiObjectManager) {
  }

  void UIObjectManager::AutoRefreshHandler::on_timer_trigger() {
    try {
      _uiObjectManager.draw();
    } catch (upan::exception &e) {
      printf("\n ui drawing failed: %s", e.ErrorMsg().c_str());
    }
  }

  void UIObjectManager::dispatch(const KeyboardEvent& event) {
    _focusedUIObject.ifPresent([&event](UIObject& uiObject) {
      uiObject.onKeyboardEvent(event);
    });
  }

  void UIObjectManager::dispatch(const MouseData& data) {
    //TODO: get focus to clicked object. Dispatch event to object under mouse x,y
    upan::mutex_guard g(_uiObjectTreeMutex);

    const int viewportX = data.x() - _rootCanvas.x();
    const int viewportY = data.y() - _rootCanvas.y();
    upan::option<UIObject&> eventObject = upan::option<UIObject&>::empty();

    if (!data.anyButtonHeld()) {
      _rootCanvas.uiObjectUnderCursor(viewportX, viewportY).ifPresent([&](UIObject& o) {
        if (data.anyButtonPressed()) {
          _focusedUIObject = upan::option<UIObject&>(o);
        }
        eventObject = upan::option<UIObject&>(o);
      });
    } else {
      eventObject = _focusedUIObject;
    }

    if (eventObject.isEmpty()) {
      if (!_mouseFocusedObject.isEmpty()) {
        _mouseFocusedObject.value().onLoseMouseFocus();
        _mouseFocusedObject = upan::option<UIObject&>::empty();
      }
      _focusedUIObject = upan::option<UIObject&>::empty();
    } else {
      if (_mouseFocusedObject.isEmpty()) {
        _mouseFocusedObject = eventObject;
        _mouseFocusedObject.value().onMouseFocus();
      } else if (&_mouseFocusedObject.value() != &eventObject.value()) {
        _mouseFocusedObject.value().onLoseMouseFocus();
        _mouseFocusedObject = eventObject;
        _mouseFocusedObject.value().onMouseFocus();
      }
      if (data.anyButtonHeld() && GraphicsContext::Instance().getResizeMode() != ResizeMode::NA) {
        eventObject.value().resize(GraphicsContext::Instance().getResizeMode(), data.deltaX(), data.deltaY());
      } else {
        const MouseEvent event(data, viewportX, viewportY);
        eventObject.value().onMouseEvent(event);
      }
    }
  }
}
