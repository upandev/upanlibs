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

#include <exception.h>

namespace upan {

  template <typename T>
  struct function_traits : function_traits<decltype(&T::operator())>
  {
  };

  template <typename R, typename C, typename... A>
  struct function_traits<R(C::*)(A...) const>
  {
    typedef R return_type;
  };

  template <typename Good>
  class result
  {
  protected:
    Good _value;
    upan::error* _error;

  public:
    result(const Good& value) : _value(value), _error(nullptr) {}
    result(const upan::error& err) : _error(new upan::error(err)) {}
    ~result() {
      delete _error;
    }

    static result<Good> bad(const char * __restrict fmsg, ...)
    {
      va_list arg;
      va_start(arg, fmsg);
      auto r = result<Good>(upan::error(fmsg, arg));
      va_end(arg);
      return r;
    }

    bool isBad() const { return _error != nullptr; }
    bool isGood() const { return !isBad(); }

    const Good& goodValue() const
    {
      if(_error)
        throw exception(XLOC, "Result is bad - can't get Good value");
      return _value;
    }

    const Good& goodValueOrThrow(const char* fileName, unsigned lineNo) const
    {
      if(_error)
        throw exception(fileName, lineNo, *_error);
      return _value;
    }

    const Good& goodValueOrElse(const Good& defaultValue) const
    {
      if(_error)
        return defaultValue;
      return _value;
    }

    template <typename LAMBDA>
    bool onGood(const LAMBDA& lambdaf)
    {
      if(_error)
        return false;
      lambdaf(_value);
      return true;
    }

    const upan::error& badValue() const
    {
      if(!_error)
        throw exception(XLOC, "result is Good - can't get Error");
      return *_error;
    }

    template <typename LAMBDA>
    bool onBad(const LAMBDA& lambdaf)
    {
      if(isGood())
        return false;
      lambdaf(*_error);
      return true;
    }
  };

  template <typename Good>
  upan::result<Good> good(const Good& value) { return upan::result<Good>(value); }

}
