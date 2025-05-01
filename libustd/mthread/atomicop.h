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

#include <stdint.h>

namespace upan {
    namespace atomic {
      class op {
      public:
        static uint32_t compare_swap(volatile uint32_t &iLock, uint32_t oldVal, uint32_t newVal);
        static uint32_t swap(volatile uint32_t &iLock, uint32_t val);
        static uint32_t add(volatile uint32_t &var, uint32_t val);
        static uint32_t bit_and(volatile uint32_t& var, uint32_t mask);
        static uint32_t bit_or(volatile uint32_t& var, uint32_t mask);
      };

      template <class T>
      class integral {
      public:
        explicit integral(T val) : _val(static_cast<uint32_t>(val)) {}

        T compare_set(T oldVal, T newVal) {
          return static_cast<T>(op::compare_swap(_val, static_cast<uint32_t>(oldVal), static_cast<uint32_t>(newVal)));
        }

        T set(T val) {
          return static_cast<T>(op::swap(_val, static_cast<uint32_t>(val)));
        }

        T get() {
          return static_cast<T>(op::add(_val, 0));
        }

        T inc() {
          return static_cast<T>(op::add(_val, 1));
        }

        T dec() {
          return static_cast<T>(op::add(_val, -1));
        }

        T add(uint32_t val) {
          return static_cast<T>(op::add(_val, val));
        }

        T bit_and(uint32_t mask) {
          return static_cast<T>(op::bit_and(_val, mask));
        }

        T bit_or(uint32_t mask) {
          return static_cast<T>(op::bit_or(_val, mask));
        }
      private:
        __volatile__ uint32_t _val;
      };
    }
}