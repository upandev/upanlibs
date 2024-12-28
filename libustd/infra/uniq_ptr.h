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

#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <option.h>

namespace upan {

  template<typename CONTAINER, typename PTR>
  class _base_uniq_ptr {
  protected:
    void destroy() {
      if (_owner && _ptr != nullptr) {
        static_cast<CONTAINER*>(this)->deletePtr();
      }
    }

  protected:
    explicit _base_uniq_ptr(PTR *ptr) : _ptr (ptr), _owner(true) {}
    _base_uniq_ptr() : _ptr(nullptr), _owner(true) {}
    _base_uniq_ptr(_base_uniq_ptr&& r) noexcept : _ptr(r.get()), _owner(true) { r.disown(); }
    _base_uniq_ptr& operator=(_base_uniq_ptr&& r) noexcept {
      destroy();
      _ptr = r.get();
      _owner = true;
      r.disown();
      return *this;
    }

    _base_uniq_ptr(const _base_uniq_ptr&) = delete;
    _base_uniq_ptr& operator=(const _base_uniq_ptr&) = delete;

    ~_base_uniq_ptr() {
      destroy();
    }

  public:
    void disown() { _owner = false; }
    bool isEmpty() const { return _ptr == nullptr; }

    PTR* get() { return _ptr; }
    const PTR* get() const { return _ptr; }


    PTR* release() {
      auto r = _ptr;
      _ptr = nullptr;
      return r;
    }

    void reset(PTR* newPtr) {
      if (_ptr != newPtr) {
        destroy();
        _ptr = newPtr;
      }
    }

  protected:
    PTR* _ptr;
    bool _owner;
  };

  template<typename T>
  class uniq_ptr : public _base_uniq_ptr<uniq_ptr<T>, T> {
  private:
    friend class _base_uniq_ptr<uniq_ptr<T>, T>;
    using _base_uniq_ptr<uniq_ptr<T>, T>::_ptr;

    void deletePtr() {
      delete _ptr;
      _ptr = nullptr;
    }

  public:
    explicit uniq_ptr(T* ptr) : _base_uniq_ptr<uniq_ptr<T>, T>(ptr) {}
    uniq_ptr() {}
    uniq_ptr(uniq_ptr &&r) noexcept = default;
    uniq_ptr &operator=(uniq_ptr &&r) noexcept = default;
    uniq_ptr(const uniq_ptr&) = delete;
    uniq_ptr &operator=(const uniq_ptr&) = delete;

    T* operator->() { return _ptr; }
    const T* operator->() const { return _ptr; }

    T& operator*() { return *_ptr; }
    const T& operator*() const { return *_ptr; }

    upan::option<T &> toOption() {
      if (_ptr) {
        return upan::option<T &>(*_ptr);
      } else {
        return upan::option<T &>::empty();
      }
    }
  };


  template<typename T>
  class uniq_ptr<T[]> : public _base_uniq_ptr<uniq_ptr<T[]>, T> {
  private:
    friend class _base_uniq_ptr<uniq_ptr<T[]>, T>;
    using _base_uniq_ptr<uniq_ptr<T[]>, T>::_ptr;

    void deletePtr() {
      delete[] _ptr;
      _ptr = nullptr;
    }

  public:
    uniq_ptr(T* ptr) : _base_uniq_ptr<uniq_ptr<T[]>, T>(ptr) {}
    uniq_ptr() = default;
    uniq_ptr(uniq_ptr &&r) noexcept = default;
    uniq_ptr &operator=(uniq_ptr &&r) noexcept = default;
    uniq_ptr(const uniq_ptr&) = delete;
    uniq_ptr &operator=(const uniq_ptr &) = delete;

    T& operator[](int index) { return _ptr[index]; }
    const T& operator[](int index) const { return _ptr[index]; }
  };
}
