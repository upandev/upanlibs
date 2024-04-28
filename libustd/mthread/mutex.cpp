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

#include <mutex.h>
#include <atomicop.h>
#include <stdlib.h>
#include <mosstd.h>
#include <newalloc.h>

namespace upan {
  mutex::mutex() : _alloc_mem_lock(nullptr), _lock(nullptr), _lockCount(0) {
    _alloc_mem_lock = aligned_alloc(4, sizeof(upan::atomic::integral<int>));
    _lock = new (_alloc_mem_lock)upan::atomic::integral<int>(0);
  }

  mutex::~mutex() {
    free(_alloc_mem_lock);
  }

  void mutex::lock() {
    if (iskernel()) {
      return;
    }

    const int newVal = getpid();
    const int oldVal = _lock->compare_set(FREE_MUTEX, newVal);
    if (oldVal == FREE_MUTEX) {
      _lockCount = 1;
    } else if (oldVal == newVal) {
      ++_lockCount;
    } else {
      waitonlock((uint64_t)_lock, FREE_MUTEX, newVal);
      _lockCount = 1;
    }
  }

  bool mutex::unlock() {
    if (iskernel()) {
      return false;
    }

    if (_lock->get() != getpid()) {
      return false;
    }

    if (_lockCount > 0) {
      --_lockCount;
    }

    if (!_lockCount) {
      _lock->set(FREE_MUTEX);
    }

    return true;
  }

  bool mutex::unlock(int pid) {
    if (iskernel()) {
      return false;
    }

    if (_lock->get() != pid) {
      return false;
    }

    _lock->set(FREE_MUTEX);

    return true;
  }
}