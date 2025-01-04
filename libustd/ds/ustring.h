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

#include <string.h>
#include <stdio.h>
#include <stdint.h>

namespace upan {

template <typename T> class list;

class string {
public:
  static const string EMPTY;
  string()
  {
    init();
  }

  string(const char* v)
  {
    if(v == nullptr)
      init();
    else
    {
      _len = strlen(v);
      _capacity = _len + 1;
      _buffer = new char[_capacity];
      strcpy(_buffer, v);
    }
  }

  string(const char* v, int len)
  {
    _len = len;
    _capacity = _len + 1;
    _buffer = new char[_capacity];
    strncpy(_buffer, v, len);
    _buffer[_len] = '\0';
  }

  string(const string& r)
  {
    _len = r.length();
    _capacity = _len + 1;
    _buffer = new char[_capacity];
    strcpy(_buffer, r.c_str());
  }

  string(string&& r) : _buffer(r._buffer), _len(r.length()), _capacity(r._capacity)
  {
    r._buffer = nullptr;
  }

  ~string()
  {
    delete[] _buffer;
  }

  string& operator=(const string& r)
  {
    _len = r.length();
    if(_capacity <= _len)
    {
      delete[] _buffer;
      _capacity = _len + 1;
      _buffer = new char[_capacity];
    }
    strcpy(_buffer, r.c_str());
    return *this;
  }

  string& operator=(string&& r)
  {
    delete[] _buffer;

    _len = r.length();
    _capacity = r._capacity;
    _buffer = r._buffer;

    r._buffer = nullptr;
    return *this;
  }

  string operator+(const string& r) const
  {
    if (r.length() == 0)
      return *this;
    string temp(*this);
    return temp += r;
  }

  string& operator+=(const string& r) {
    if (r.length() > 0) {
      expandAndCopy(r.c_str(), r.length());
    }
    return *this;
  }

  string& operator+=(const char* r) {
    const int rlen = strlen(r);
    if (rlen > 0) {
      expandAndCopy(r, rlen);
    }
    return *this;
  }

  bool operator<(const string& r) const
  {
    return strcmp(c_str(), r.c_str()) < 0;
  }

  bool operator==(const string& r) const
  {
    return strcmp(c_str(), r.c_str()) == 0;
  }

  bool operator!=(const string& r) const
  {
    return !(*this == r);
  }

  const char* c_str() const
  {
    return _buffer;
  }

  int length() const { return _len; }
  bool empty() const { return _len == 0; }

  const char operator[](int index) const;
  int find(char c) const;
  int find(const upan::string& str) const;
  string substr(int start, int len) const;
  string substr(int start) const;
  bool split(char c, string& p1, string& p2) const;

  template <typename Tokens>
  void tokenize(const upan::string& delim, bool filterEmpty, Tokens& tokens) const {
    upan::string nextToken = *this;
    while(true) {
      int pos = nextToken.find(delim);
      if (pos < 0) {
        if (!filterEmpty || nextToken.length() > 0) {
          tokens.push_back(nextToken);
        }
        break;
      }
      const upan::string& firstToken = nextToken.substr(0, pos);
      if (!filterEmpty || firstToken.length() > 0) {
        tokens.push_back(firstToken);
      }
      nextToken = nextToken.substr(pos + delim.length());
    }
  }

private:
  void init()
  {
    _capacity = 8;
    _len = 0;
    _buffer = new char[_capacity];
    _buffer[0] = '\0';
  }

  void expandAndCopy(const char* r, int rlen) {
    _len += rlen;
    if(_capacity <= _len) {
      _capacity = (_len + 1) * 1.5;
      char* temp = new char[_capacity];
      strcpy(temp, _buffer);
      delete[] _buffer;
      _buffer = temp;
    }
    strcat(_buffer, r);
  }

private:
  char* _buffer;
  int   _len;
  int   _capacity;

public:
  static upan::string to_string(uint64_t uiNumber) {
    char strNumber[128];
    unsigned i = 0;

    do {
      strNumber[i++] = (uiNumber % 10) + 0x30;
      uiNumber /= 10;
      if(i == 128)
        return "";
    }
    while(uiNumber) ;

    strNumber[i] = '\0';
    strreverse(strNumber);
    return strNumber;
  }
};

};
