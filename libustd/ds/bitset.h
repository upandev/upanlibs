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

#include <stdlib.h>
#include <string.h>
#include <exception.h>

namespace upan {

template <int SIZE>
class bitset {
public:
  bitset();
  template <typename V>
  bitset(V val);

  void set(int i, bool v);
  void set(int i);

  void reset();
  void reset(int i);

  void flip();
  void flip(int i);

  bool test(int i) const;
  bool none() const;
  bool any() const;
  bool all() const;

  int allocate(int start, int end);
  int allocate();

private:
  static constexpr int BITS_PER_ELEMENT = sizeof(uint64_t) * 8;
  static constexpr int BIT_ARRAY_SIZE = (SIZE + BITS_PER_ELEMENT - 1) / BITS_PER_ELEMENT;
  uint64_t _bit_array[BIT_ARRAY_SIZE];
};

template <int SIZE>
bitset<SIZE>::bitset() {
  memset(_bit_array, 0, sizeof(_bit_array));
}

template <int SIZE>
template <typename V>
bitset<SIZE>::bitset(V val) : bitset() {
  if (0 < BIT_ARRAY_SIZE) {
    _bit_array[0] = val;
  }
}

template <int SIZE>
void bitset<SIZE>::set(int i, bool v) {
  const auto array_index = i / BITS_PER_ELEMENT;
  const auto bit_pos = i % BITS_PER_ELEMENT;
  if (array_index < BIT_ARRAY_SIZE) {
    const uint64_t val = ((uint64_t)1) << bit_pos;
    if (v) {
      _bit_array[array_index] |= val;
    } else {
      _bit_array[array_index] &= ~val;
    }
  }
}

template <int SIZE>
void bitset<SIZE>::set(int i) {
  set(i, true);
}

template <int SIZE>
void bitset<SIZE>::reset() {
  memset(_bit_array, 0, sizeof(_bit_array));
}

template <int SIZE>
void bitset<SIZE>::reset(int i) {
  set(i, false);
}

template <int SIZE>
void bitset<SIZE>::flip() {
  for(int i = 0; i < BIT_ARRAY_SIZE; ++i) {
    _bit_array[i] = ~_bit_array[i];
  }
}

template <int SIZE>
void bitset<SIZE>::flip(int i) {
  set(i, !test(i));
}

template <int SIZE>
bool bitset<SIZE>::test(int i) const {
  const auto array_index = i / BITS_PER_ELEMENT;
  const auto bit_pos = i % BITS_PER_ELEMENT;
  if (array_index < BIT_ARRAY_SIZE) {
    return (_bit_array[array_index] >> bit_pos) & 1;
  }
  return false;
}

template <int SIZE>
bool bitset<SIZE>::none() const {
  for(int i = 0; i < BIT_ARRAY_SIZE; ++i) {
    if (_bit_array[i] != 0) {
      return false;
    }
  }
  return true;
}

template <int SIZE>
bool bitset<SIZE>::any() const {
  return !none();
}

template <int SIZE>
bool bitset<SIZE>::all() const {
  for(int i = 0; i < BIT_ARRAY_SIZE; ++i) {
    if (_bit_array[i] != UINT64_MAX) {
      return false;
    }
  }
  return true;
}

template <int SIZE>
int bitset<SIZE>::allocate(int start, int end) {
  const auto start_array_index = start / BITS_PER_ELEMENT;
  const auto start_bit_pos = start % BITS_PER_ELEMENT;

  const auto end_array_index = end / BITS_PER_ELEMENT;
  const auto end_bit_pos = end % BITS_PER_ELEMENT;

  for (int i = start_array_index; i < BIT_ARRAY_SIZE && i <= end_array_index; ++i) {
    const auto v = _bit_array[i];
    if (v != UINT64_MAX) {
      const auto j_start = i == start_array_index ? start_bit_pos : 0;
      const auto j_end = i == end_array_index ? end_bit_pos : BITS_PER_ELEMENT - 1;
      for (int j = j_start; j <= j_end; ++j) {
        if (((v >> j) & 1) == 0) {
          _bit_array[i] |= (1 << j);
          return i * BITS_PER_ELEMENT + j;
        }
      }
    }
  }
  throw upan::exception(XLOC, "no free ports in range [%d-%d]", start, end);
}

template <int SIZE>
int bitset<SIZE>::allocate() {
  return allocate(0, SIZE - 1);
}

}