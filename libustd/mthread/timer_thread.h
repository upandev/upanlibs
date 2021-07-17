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

#pragma once

#include <stdint.h>
#include <mutex.h>
#include <option.h>

namespace upan {
    class timer_thread {
    public:
      enum state_t {
        not_running,
        running,
        paused,
        stopping,
        stopped
      };

      timer_thread(uint32_t interval_ms);
      virtual ~timer_thread();

      virtual void on_timer_trigger() = 0;

      uint32_t interval() const {
        return _timer_interval_ms;
      }

      state_t state() {
        return _state.get();
      }

      void run();
      void pause();
      void stop();
      bool is_active();

      void set_error(const upan::error& e);

      bool has_error() const {
        return _error.isEmpty();
      }

      const upan::option<upan::error>& get_error() const {
        return _error;
      }

    private:
      const uint32_t _timer_interval_ms;
      atomic::integral<state_t> _state;
      mutex _timer_mutex;
      upan::option<upan::error> _error;

      friend class _timer_thread_termination_guard;
    };
}