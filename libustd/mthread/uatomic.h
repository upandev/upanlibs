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

#include <atomicop.h>

namespace upan {
    namespace atomic {
      template <class T>
      class integral {
      public:
        explicit integral(T val) : _val(static_cast<uint32_t>(val)) {}

        T compare_set(T oldVal, T newVal) {
          return static_cast<T>(atomic_compare_swap(&_val, static_cast<uint32_t>(oldVal), static_cast<uint32_t>(newVal)));
        }

        T swap(T val) {
          return static_cast<T>(atomic_swap(&_val, static_cast<uint32_t>(val)));
        }

        void set(T val) {
          atomic_set(&_val, static_cast<uint32_t>(val));
        }

        T get() {
          return static_cast<T>(atomic_get(&_val));
        }

        T inc() {
          return static_cast<T>(atomic_add(&_val, 1));
        }

        T dec() {
          return static_cast<T>(atomic_add(&_val, -1));
        }

        T add(uint32_t val) {
          return static_cast<T>(atomic_add(&_val, val));
        }

        T bit_and(uint32_t mask) {
          return static_cast<T>(atomic_bit_and(&_val, mask));
        }

        T bit_or(uint32_t mask) {
          return static_cast<T>(atomic_bit_or(&_val, mask));
        }
      private:
        alignas(4) uint32_t _val;
      };
    }
}