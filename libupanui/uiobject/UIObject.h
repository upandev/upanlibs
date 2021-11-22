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
#include <stdio.h>
#include <usfncontext.h>
#include <option.h>
#include <set.h>
#include <atomicop.h>

namespace upanui {

  class UIObject {
  protected:
    virtual ~UIObject() {}

  public:
    UIObject(const int x, const int y, const uint32_t width, const uint32_t height);

    UIObject(const UIObject&) = delete;
    UIObject& operator=(const UIObject&) = delete;


    int x() const { return _x; }
    int y() const { return _y; }
    uint32_t width() const { return _width; }
    uint32_t height() const { return _height; }

    void x(const int);
    void y(const int);
    void width(const uint32_t);
    void height(const uint32_t);

    upan::option<UIObject&> parent();
    const upan::set<UIObject*>& children();
    void add(UIObject& child);
    void remove();

    bool positionChanged() {
      return _positionChanged.get();
    }
    void positionChanged(bool v) {
      _positionChanged.set(v);
    }

    bool sizeChanged() {
      return _sizeChanged.get();
    }
    void sizeChanged(bool v) {
      _sizeChanged.set(v);
    }

    bool contentChanged() {
      return _contentChanged.get();
    }
    void contentChanged(bool v) {
      _contentChanged.set(v);
    }

  private:
    int _x;
    int _y;
    uint32_t _width;
    uint32_t _height;

    upan::atomic::integral<bool> _positionChanged;
    upan::atomic::integral<bool> _sizeChanged;
    upan::atomic::integral<bool> _contentChanged;

    friend class UIObjectManager;
  };
}
