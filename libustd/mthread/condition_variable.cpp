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
#include <condition_variable.h>
#include "exception.h"

namespace upan {
  condition_variable::condition_variable() {
    pthread_cond_init(&_cond, nullptr);
  }

  condition_variable::condition_variable(const pthread_condattr_t& attr) {
    pthread_cond_init(&_cond, &attr);
  }

  condition_variable::~condition_variable() {
    pthread_cond_destroy(&_cond);
  }

  void condition_variable::wait(mutex& m) {
    if (pthread_cond_wait(&_cond, &m._m)) {
      throw upan::exception(XLOC, "cv wait interrupted");
    }
  }

  void condition_variable::wait(mutex& m, const struct timespec* abstime) {
    if (pthread_cond_timedwait(&_cond, &m._m, abstime)) {
      throw upan::exception(XLOC, "cv wait timeout/interrupted");
    }
  }

  void condition_variable::notify_one() {
    pthread_cond_signal(&_cond);
  }

  void condition_variable::notify_all() {
    pthread_cond_broadcast(&_cond);
  }
}