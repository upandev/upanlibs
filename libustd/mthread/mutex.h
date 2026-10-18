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
#include <uatomic.h>
#include <pthread.h>

namespace upan{
    class mutex {
    private:
      pthread_mutex_t _m;

    public:
      mutex();
      explicit mutex(pthread_mutex_t& m);
      ~mutex();

      mutex(const mutex&) = delete;
      mutex(mutex&) = delete;
      mutex& operator=(const mutex&) = delete;
      mutex& operator=(mutex&) = delete;

      void lock();
      int trylock();
      bool unlock();
      bool unlock(int pid);
    };

    class mutex_guard {
    private:
      mutex_guard() = delete;
    public:
      mutex_guard(mutex& m) : _m(m) {
        _m.lock();
      }
      mutex_guard(const mutex& m) : mutex_guard(const_cast<mutex&>(m)) {
      }
      ~mutex_guard() {
        _m.unlock();
      }
    private:
      mutex& _m;
    };
}