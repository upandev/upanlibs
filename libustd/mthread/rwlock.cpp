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

#include <rwlock.h>

namespace upan {
  void rwlock::read_lock() {
    mutex_guard g(_m);

    const auto pid = getpid();

    _cv.waitc(_m, [&] {
      return _active_writer == NO_ACTIVE_WRITER || _active_writer == pid;
    });

    ++_readers[pid];
  }

  void rwlock::read_unlock() {
    mutex_guard g(_m);

    const auto pid = getpid();

    auto i = _readers.find(pid);
    if (i == _readers.end()) {
      //throw upan::exception(XLOC, "read unlock called on unknown pid: %d", pid);
      return;
    }

    --(i->second);

    if (i->second == 0) {
      _readers.erase(i);
    }

    if (_readers.empty() && _active_writer == NO_ACTIVE_WRITER) {
      _cv.notify_one();
    }
  }

  void rwlock::write_lock() {
    mutex_guard g(_m);

    const auto pid = getpid();

    _cv.waitc(_m, [&] {
      return _active_writer == pid || (_active_writer == NO_ACTIVE_WRITER && _readers.empty());
    });

    _active_writer = pid;
    ++_active_writer_lock_count;
  }

  void rwlock::write_unlock() {
    mutex_guard g(_m);

    const auto pid = getpid();

    if (_active_writer != pid) {
      return;
    }

    --_active_writer_lock_count;
    if (_active_writer_lock_count == 0) {
      _active_writer = NO_ACTIVE_WRITER;
      _cv.notify_all();
    }
  }
}