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
  atomic::integral<int> condition_variable::_global_cv_id_seq(0);

  condition_variable::condition_variable() : _id(_global_cv_id_seq.inc()) {
  }

  void condition_variable::wait(mutex& m) {
    if (!waitqueue(_id, &m, nullptr)) {
      m.lock();
    } else {
      throw upan::exception(XLOC, "cv wait interrupted");
    }
  }

  void condition_variable::wait(mutex& m, const struct timeval* timeout) {
    if(!waitqueue(_id, &m, timeout)) {
      m.lock();
    } else {
      throw upan::exception(XLOC, "cv wait timeout/interrupted");
    }
  }

  void condition_variable::notify_one() {
    waitdequeue(_id, false);
  }

  void condition_variable::notify_all() {
    waitdequeue(_id, true);
  }
}