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

#include <atomicop.h>

namespace upan {
  namespace atomic {
    uint32_t op::compare_swap(volatile uint32_t &iLock, uint32_t oldVal, uint32_t newVal) {
      return __sync_val_compare_and_swap(&iLock, oldVal, newVal);
    }

    uint32_t op::swap(volatile uint32_t &iLock, uint32_t val) {
      return __atomic_exchange_n(&iLock, val, __ATOMIC_SEQ_CST);
    }

    uint32_t op::add(volatile uint32_t &var, uint32_t val) {
      return __atomic_fetch_add(&var, val, __ATOMIC_SEQ_CST);
    }

    uint32_t op::bit_and(volatile uint32_t& var, uint32_t mask) {
      return __atomic_fetch_and(&var, mask, __ATOMIC_SEQ_CST);
    }

    uint32_t op::bit_or(volatile uint32_t& var, uint32_t mask) {
      return __atomic_fetch_or(&var, mask, __ATOMIC_SEQ_CST);
    }
  }
}