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

#include <thread.h>
#include <mosstd.h>
#include <exception.h>

namespace upan {
  thread::thread() : _state(not_running), _error(upan::option<upan::error>::empty()) {
  }

  thread::~thread() {
    stop();
  }

  class _thread_termination_guard {
  public:
    _thread_termination_guard(thread* t) : _t(t) {
    }
    ~_thread_termination_guard() {
      _t->_state.set(thread::stopped);
    }
  private:
    thread* _t;
  };

  void thread_callback(void* obj) {
    auto t = static_cast<thread*>(obj);
    _thread_termination_guard accessor(t);
    try {
      t->run();
    } catch(const exception& e) {
      t->set_error(e.Error());
    }
  }

  void thread::start() {
    mutex_guard g(_t_mutex);
    switch(_state.get()) {
      case not_running:
        _state.set(running);
        exect(thread_callback, this);
        break;
      case paused:
        _state.set(running);
        break;
      case running:
        throw exception(XLOC, "timer is already running");
      case stopping:
        throw exception(XLOC, "timer has stopping - can't run again");
      case stopped:
        throw exception(XLOC, "timer has stopped - can't run again");
      default:
        throw exception(XLOC, "timer is in unknown state: %d", _state);
    }
  }

  void thread::pause() {
    mutex_guard g(_t_mutex);
    switch(_state.get()) {
      case running:
        _state.set(paused);
        break;
      case paused:
        break;
      case not_running:
        throw exception(XLOC, "timer is not running - can't pause");
      case stopped:
        throw exception(XLOC, "timer has stopped - can't pause");
      default:
        throw exception(XLOC, "timer is in unknown state: %d", _state);
    }
  }

  bool thread::is_active() {
    return _state.get() == running || _state.get() == paused;
  }

  void thread::stop() {
    if (_state.get() != not_running && _state.get() != stopped) {
      _state.set(stopping);
      while(_state.get() != stopped) {
        sleepms(10);
      }
    }
  }

  void thread::set_error(const upan::error& e) {
    mutex_guard g(_t_mutex);
    _error = upan::option<upan::error>(e);
  }
}
