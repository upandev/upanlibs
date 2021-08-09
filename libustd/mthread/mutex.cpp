/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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

namespace upan {
  mutex::mutex() : _lock(0), _processID(FREE_MUTEX) {
  }

  mutex::~mutex() {
  }

  void mutex::acquire() {
    while (true) {
      if (_lock.set(1) == 0) {
        break;
      }
      yield();
    }
  }

  void mutex::release() {
    _lock.set(0);
  }

  bool mutex::lock(bool bBlock) {
    if (iskernel()) {
      return false;
    }

    __volatile__ int val;

    while (true) {
      acquire();

      val = getpid();

      if (_processID != FREE_MUTEX && _processID != val) {
        if (isprocessalive(_processID)) {
          release();

          if (!bBlock)
            return false;

          yield();
          continue;
        } else {
          _processID = FREE_MUTEX;
        }
      }

      if (_processID == FREE_MUTEX)
        _processID = val;

      release();
      break;
    }

    return true;
  }

  bool mutex::unlock() {
    if (iskernel()) {
      return false;
    }

    acquire();

    __volatile__ int pid = getpid();

    if (_processID != pid) {
      release();
      return false;
    }

    _processID = FREE_MUTEX;

    release();

    return true;
  }

  bool mutex::unlock(int pid) {
    if (iskernel()) {
      return false;
    }

    acquire();

    if (_processID != pid) {
      release();
      return false;
    }

    _processID = FREE_MUTEX;

    release();

    return true;
  }
}