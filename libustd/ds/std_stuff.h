/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2015 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
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

#include <exception.h>
#include <pair.h>
#include <_tree.h>

namespace std {
  template<typename T>
  class initializer_list {
  public:
    using value_type = T;
    using reference = const T &;
    using const_reference = const T &;
    using size_type = size_t;
    using iterator = const T *;
    using const_iterator = const T *;

  private:
    iterator _begin;
    size_type _size;

    // The compiler can call a private constructor.
    constexpr initializer_list(const_iterator begin, size_type size)
            : _begin(begin), _size(size) {}

  public:
    constexpr initializer_list() noexcept: _begin(nullptr), _size(0) {}

    // Number of elements.
    constexpr size_type size() const noexcept { return _size; }

    // First element.
    constexpr const_iterator begin() const noexcept { return _begin; }

    // One past the last element.
    constexpr const_iterator end() const noexcept { return begin() + size(); }
  };
};