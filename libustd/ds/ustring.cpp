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

#include <exception.h>
#include <list.h>

namespace upan {

const string string::EMPTY("");

const char string::operator[](int index) const {
  if(index < 0 || index > length())
    throw exception(XLOC, "accessing string index (%d) out of range", index);
  return c_str()[index];
}

int string::find(char c) const {
  for(int i = 0; i < _len; ++i) {
    if (_buffer[i] == c) {
      return i;
    }
  }
  return -1;
}

int string::find(const upan::string& str) const {
  if (str.length() == 0) {
    return -1;
  }

  if (str.length() == 1) {
    return find(str[0]);
  }

  int s = 0;

  while (true) {
    const int remaining = _len - s;
    if (remaining < str.length()) {
      return -1;
    }

    int i;
    const char *in = str.c_str();
    for (i = 0; i < str.length(); ++i) {
      if (_buffer[s + i] != in[i]) {
        break;
      }
    }

    if (i == str.length()) {
      return s;
    } else {
      ++s;
    }
  }
}

string string::substr(int start, int len) const {
  if (start < 0 || len <= 0 || start >= _len) {
    return "";
  }
  const int remaining = _len - start;
  if (len > remaining) {
    len = remaining;
  }
  return {_buffer + start, len};
}

string string::substr(int start) const {
  return substr(start, _len - start);
}

bool string::split(char c, string& p1, string& p2) const {
  int pos = find(c);
  if (pos < 0) {
    return false;
  }
  p1 = substr(0, pos);
  p2 = substr(pos+1);
  return true;
}

void string::pop_back() {
  if (_len > 0) {
    --_len;
    _buffer[_len] = '\0';
  }
}

upan::string upan::string::to_string(uint64_t uiNumber) {
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

upan::string upan::string::trim(const upan::string& line) {
  const char* l = line.c_str();

  int s = 0;
  for(int i = 0; i < line.length(); ++i) {
    if (!isspace(l[i])) {
      s = i;
      break;
    }
  }

  int e = line.length() - 1;
  for(int i = line.length() - 1; i >= 0; --i) {
    if (!isspace(l[i])) {
      e = i;
      break;
    }
  }

  if (e < s) {
    return EMPTY;
  }

  return { l + s, e - s + 1};
}
}
