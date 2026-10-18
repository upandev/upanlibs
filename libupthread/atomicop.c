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

uint32_t atomic_compare_swap(void* lock, uint32_t oldVal, uint32_t newVal) {
  return __sync_val_compare_and_swap((uint32_t*)lock, oldVal, newVal);
}

uint32_t atomic_swap(void* lock, uint32_t val) {
  return __atomic_exchange_n((uint32_t*)lock, val, __ATOMIC_SEQ_CST);
}

uint32_t atomic_add(void* var, uint32_t val) {
  return __atomic_fetch_add((uint32_t*)var, val, __ATOMIC_SEQ_CST);
}

uint32_t atomic_get(void* var) {
  return __atomic_load_n((uint32_t*)var, __ATOMIC_SEQ_CST);
}

void atomic_set(void* var, uint32_t val) {
  __atomic_store_n((uint32_t*)var, val, __ATOMIC_SEQ_CST);
}

uint32_t atomic_bit_and(void* var, uint32_t mask) {
  return __atomic_fetch_and((uint32_t*)var, mask, __ATOMIC_SEQ_CST);
}

uint32_t atomic_bit_or(void* var, uint32_t mask) {
  return __atomic_fetch_or((uint32_t*)var, mask, __ATOMIC_SEQ_CST);
}
