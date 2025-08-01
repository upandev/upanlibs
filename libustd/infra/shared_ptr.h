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
#include <atomicop.h>

namespace upan {

  template<typename CONTAINER, typename PTR>
  class _base_shared_ptr {
  private:
    void destroy() {
      if (_refCount && _refCount->dec() == 1) {
        static_cast<CONTAINER*>(this)->deletePtr();

        delete _refCount;
        _refCount = nullptr;
      }
    }

  protected:
    explicit _base_shared_ptr(PTR* ptr) : _ptr(ptr), _refCount(ptr ? new upan::atomic::integral<int>(1) : nullptr) {
    }

    _base_shared_ptr() : _ptr(nullptr), _refCount(nullptr) {
    }

    _base_shared_ptr(const _base_shared_ptr& r) : _ptr(r._ptr), _refCount(r._refCount) {
      if (_refCount) _refCount->inc();
    }

    _base_shared_ptr& operator=(const _base_shared_ptr& r) {
      if (this != &r) {
        destroy();
        _ptr = r._ptr;
        _refCount = r._refCount;
        if (_refCount) _refCount->inc();
      }
      return *this;
    }

    _base_shared_ptr(_base_shared_ptr&& r) : _ptr(r._ptr), _refCount(r._refCount) {
      r._ptr = nullptr;
      r._refCount = nullptr;
    }

    _base_shared_ptr& operator=(_base_shared_ptr&& r) {
      destroy();
      _ptr = r._ptr;
      _refCount = r._refCount;
      r._ptr = nullptr;
      r._refCount = nullptr;
      return *this;
    }

    ~_base_shared_ptr() {
      destroy();
    }

  public:
    bool isEmpty() const { return _ptr == nullptr; }

    PTR* get() { return _ptr; }
    const PTR* get() const { return _ptr; }

    void reset(PTR* newPtr) {
      if (_ptr != newPtr) {
        destroy();
        _ptr = newPtr;
        _refCount = _ptr ? new upan::atomic::integral<int>(1) : nullptr;
      }
    }

  protected:
    PTR* _ptr;
    upan::atomic::integral<int>* _refCount;
  };

  template<typename T>
  class shared_ptr : public _base_shared_ptr<shared_ptr<T>, T> {
  private:
    friend class _base_shared_ptr<shared_ptr<T>, T>;
    using _base_shared_ptr<shared_ptr<T>, T>::_ptr;

    void deletePtr() {
      delete _ptr;
      _ptr = nullptr;
    }

  public:
    shared_ptr(T* ptr) : _base_shared_ptr<shared_ptr<T>, T>(ptr) {}
    shared_ptr() {}
    shared_ptr(const shared_ptr&r) = default;
    shared_ptr& operator=(const shared_ptr& r) = default;
    shared_ptr(shared_ptr&& r) = default;
    shared_ptr &operator=(shared_ptr&& r) = default;

    bool operator<(const shared_ptr& r) const {
      return _ptr < r._ptr;
    }

    T* operator->() { return _ptr; }
    const T* operator->() const { return _ptr; }

    T& operator*() { return *_ptr; }
    const T &operator*() const { return *_ptr; }

    upan::option<T&> toOption() {
      if (_ptr) {
        return upan::option<T&>(*_ptr);
      } else {
        return upan::option<T&>::empty();
      }
    }
  };

  template<typename T>
  class shared_ptr<T[]> : public _base_shared_ptr<shared_ptr<T[]>, T>{
  private:
    friend class _base_shared_ptr<shared_ptr<T[]>, T>;
    using _base_shared_ptr<shared_ptr<T[]>, T>::_ptr;

    void deletePtr() {
      delete[] _ptr;
      _ptr = nullptr;
    }

  public:
    explicit shared_ptr(T* ptr) : _base_shared_ptr<shared_ptr<T[]>, T>(ptr) {}
    shared_ptr() {}
    shared_ptr(const shared_ptr&r) = default;
    shared_ptr& operator=(const shared_ptr& r) = default;
    shared_ptr(shared_ptr&& r) = default;
    shared_ptr &operator=(shared_ptr&& r) = default;

    T& operator[](int index) { return _ptr[index]; }
    const T& operator[](int index) const { return _ptr[index]; }
  };
}
