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

#include <condition_variable.h>

namespace upan {
  class rwlock {
  public:
    rwlock() : _readers_count(0), _active_writer(false) {}

    void read_lock();
    void read_unlock();
    void write_lock();
    void write_unlock();

  private:
    upan::mutex _m;
    upan::condition_variable _cv;
    int _readers_count;
    bool _active_writer;
  };

  class rlock_gaurd {
  private:
    rlock_gaurd() = delete;
  public:
    rlock_gaurd(rwlock& lock) : _lock(lock) {
      _lock.read_lock();
    }
    rlock_gaurd(const rwlock& lock) : rlock_gaurd(const_cast<rwlock&>(lock)) {
    }
    ~rlock_gaurd() {
      _lock.read_unlock();
    }
  private:
    rwlock& _lock;
  };

  class wlock_gaurd {
  private:
    wlock_gaurd() = delete;
  public:
    wlock_gaurd(rwlock& lock) : _lock(lock) {
      _lock.write_lock();
    }
    wlock_gaurd(const rwlock& lock) : wlock_gaurd(const_cast<rwlock&>(lock)) {
    }
    ~wlock_gaurd() {
      _lock.write_unlock();
    }
  private:
    rwlock& _lock;
  };
}