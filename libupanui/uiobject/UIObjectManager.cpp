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
#include <RootCanvas.h>

namespace upanui {
  UIObjectManager::UIObjectManager(RootCanvas& rootCanvas) : _rootCanvas(rootCanvas), _drawTimerThread(*this) {
    _parentChildMap.insert(ParentChildMap::value_type(&_rootCanvas, upan::set<UIObject*>()));
    _drawTimerThread.start();
    queueForRedraw(_rootCanvas);
  }

  UIObjectManager::~UIObjectManager() {
    _drawTimerThread.stop();
    destroy(_rootCanvas);
  }

  upan::option<UIObject&> UIObjectManager::parent(UIObject& child) {
    upan::mutex_guard g(_uiObjectTreeMutex);

    auto i = _childParentMap.find(&child);
    if (i == _childParentMap.end()) {
      return upan::option<UIObject&>::empty();
    } else {
      return upan::option<UIObject&>(*i->second);
    }
  }

  const upan::set<UIObject*>& UIObjectManager::children(UIObject& parent) {
    upan::mutex_guard g(_uiObjectTreeMutex);
    return _parentChildMap[&parent];
  }

  void UIObjectManager::add(UIObject& parent, UIObject& child) {
    upan::mutex_guard g(_uiObjectTreeMutex);

    if (&child == &_rootCanvas) {
      throw upan::exception(XLOC, "RootCanvas can't be a child UIObject");
    }

    if (&parent == &child) {
      throw upan::exception(XLOC, "parent and child can't be same");
    }

    auto i = _childParentMap.find(&child);
    if (i != _childParentMap.end()) {
      throw upan::exception(XLOC, "UIObject is already a child of another UIObject");
    }
    _parentChildMap[&parent].insert(&child);
    _childParentMap[&child] = &parent;
  }

  void UIObjectManager::remove(UIObject& child) {
    upan::mutex_guard g(_uiObjectTreeMutex);
    parent(child).ifPresent([&](UIObject& parent) {
      _parentChildMap[&parent].erase(&child);
    });
  }

  void UIObjectManager::destroy(UIObject& uiObject) {
    upan::mutex_guard g(_uiObjectTreeMutex);

    remove(uiObject);
    _childParentMap.erase(&uiObject);

    _parentChildMap[&uiObject].foreach([this](UIObject* child) { destroy(*child); });
    _parentChildMap.erase(&uiObject);

    upan::mutex_guard g1(_uiObjectQueueMutex);
    _modifiedUIObjects.erase(&uiObject);

    delete &uiObject;
  }

  void UIObjectManager::queueForRedraw(UIObject& uiObject) {
    upan::mutex_guard g(_uiObjectQueueMutex);

    if (_modifiedUIObjects.size() == MAX_OBJECTS_UPDATE_QUEUE) {
      _modifiedUIObjects.clear();
      _modifiedUIObjects.insert(&_rootCanvas);
    } else {
      _modifiedUIObjects.insert(&uiObject);
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
  }

  UIObjectManager::DrawTimerThread::DrawTimerThread(UIObjectManager& uiObjectManager) : upan::timer_thread(50), _uiObjectManager(uiObjectManager) {
  }

  void UIObjectManager::DrawTimerThread::on_timer_trigger() {
    _uiObjectManager.draw();
  }
}
