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

#include <metrics.h>
#include <dtime.h>
#include <mosstd.h>

namespace upan {
  metrics *metrics::_instance = nullptr;

  void metrics::create() {
    if (!_instance) {
      static metrics instance;
      _instance = &instance;
    }
  }

  metrics &metrics::instance() {
    if (!_instance) {
      throw exception(XLOC, "metrics instance not created yet!");
    }
    return *_instance;
  }

  metrics::stats &metrics::get(const string &name) {
    mutex_guard g(_mutexStats);
    return _stats[name][getpid()];
  }

  void metrics::remove(const string &name) {
    mutex_guard g(_mutexStats);
    _stats.erase(name);
  }

  uint32_t metrics::count(const string &name) {
    mutex_guard g(_mutexStats);
    uint32_t count = 0;
    for(auto& s : _stats[name]) {
      count += s.second.count();
    }
    return count;
  }

  double metrics::avg(const string &name) {
    mutex_guard g(_mutexStats);
    uint32_t sum = 0;
    uint32_t count = 0;
    for(auto& s : _stats[name]) {
      count += s.second.count();
      sum += s.second.sum();
    }
    return double(sum) / count;
  }

  metrics::stats::stats() : _count(0), _sum(0) {}

  void metrics::stats::start() {
    _tick = btime();
  }

  void metrics::stats::end() {
    _sum += (btime() - _tick);
    ++_count;
  }

  uint32_t metrics::stats::count() {
    return _count;
  }

  uint32_t metrics::stats::sum() {
    return _sum;
  }

  double metrics::stats::avg() {
    return double(_sum) / _count;
  }
}