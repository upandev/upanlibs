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

#include <utility.h>

namespace upan {
  template <typename RT, typename... Args>
  class function {
  private:
    class _icallable {
    public:
      virtual ~_icallable() {}
      virtual RT invoke(Args... args) const = 0;
    };

    template <typename T>
    class _callable : public _icallable {
    public:
      _callable(const T&& t) : _t(move(t)) {
      }
      RT invoke(Args... args) const override {
        return _t(args...);
      }
    private:
      const T _t;
    };
  public:
    template <typename T>
    function(const T&& t) {
      _c.reset(new _callable<T>(move(t)));
    }

    template <typename T>
    function& operator=(const T&& t) {
      _c.reset(new _callable<T>(move(t)));
      return *this;
    }
    RT operator()(Args... args) const {
      return _c->invoke(args...);
    }
  private:
    uniq_ptr<_icallable> _c;
  };
}