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

#include <map.h>
#include <set.h>
#include <mutex.h>

class GraphicsVideo;

namespace upanui {
  class UIObject;
  class RootCanvas;

  class UIObjectManager {
  private:
    UIObjectManager(RootCanvas& rootCanvas);

    upan::option<UIObject&> parent(UIObject& child);
    const upan::set<UIObject*>& children(UIObject& parent);
    void add(UIObject& parent, UIObject& child);
    void remove(UIObject& child);
    void destroy(UIObject& parent);
    static void directDelete(UIObject& uiObject);

  private:
    typedef upan::map<UIObject*, upan::set<UIObject*>> ParentChildMap;
    typedef upan::map<UIObject*, UIObject*> ChildParentMap;
    ParentChildMap _parentChildMap;
    ChildParentMap _childParentMap;
    RootCanvas& _rootCanvas;

    upan::mutex _managerMutex;

    friend class GraphicsContext;
    friend class UIObject;
    friend class ::GraphicsVideo;
  };
}
