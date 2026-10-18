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

#include <spinmutex.h>
#include <uatomic.h>
#include <newalloc.h>
#include <mosstd.h>

namespace upan {
  spinmutex::spinmutex() : _lock(FREE_MUTEX), _lockCount(0) {
  }

  spinmutex::~spinmutex() {
  }

  void spinmutex::lock() {
    const int newVal = getlockownerid();

    while (true) {
      const int oldVal = _lock.compare_set(FREE_MUTEX, newVal);
      if (oldVal == FREE_MUTEX) {
        _lockCount = 1;
        return;
      } else if (oldVal == newVal) {
        ++_lockCount;
        return;
      } else {
        while (_lock.get() != FREE_MUTEX) {
          __asm__ __volatile__("pause");
        }
      }
    }
  }

  bool spinmutex::unlock() {
    const int val = getlockownerid();

    if (_lock.get() != val) {
      return false;
    }

    if (_lockCount > 0) {
      --_lockCount;
    }

    if (!_lockCount) {
      _lock.set(FREE_MUTEX);
    }

    return true;
  }
}