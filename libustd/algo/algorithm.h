/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2015 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
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
#ifndef _ALGO_H_
#define _ALGO_H_

namespace upan {

template <typename T>
class equal_to final
{
  public:
    equal_to(const T& value) : _value(value) {}
    bool operator()(const T& rhs) const { return _value == rhs; }
  private:
    const T _value;
};

template <typename IT>
IT find(IT begin, IT end, const typename IT::value_type& value)
{
  auto i = begin;
  for(; i != end; ++i)
    if(*i == value)
      break;
  return i;
}

template <typename IT, typename MATCH>
IT find_if(IT begin, IT end, MATCH match)
{
  auto i = begin;
  for(; i != end; ++i)
    if(match(*i))
      break;
  return i;
}

template <typename N>
const N& max(const N& n) {
  return n;
}

template <typename N, typename... Ns>
const N& max(const N& n1, const N& n2, const Ns&... ns) {
  return max(n1 > n2 ? n1 : n2, ns...);
}

template <typename N>
const N& min(const N& n) {
  return n;
}

template <typename N, typename... Ns>
const N& min(const N& n1, const N& n2, const Ns&... ns) {
  return min(n1 < n2 ? n1 : n2, ns...);
}

template <typename N>
uint32_t hash32(const N& n) {
  return n;
}

template <typename N1, typename N2, typename... Ns>
uint32_t hash32(const N1& n1, const N2& n2, const Ns&... ns) {
  return hash32(((n1 << 1) | (n1 >> 31)) ^ n2, ns...);
}

}

#endif
