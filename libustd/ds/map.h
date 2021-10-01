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
#ifndef _UPAN_MAP_H_
#define _UPAN_MAP_H_

#include <exception.h>
#include <pair.h>
#include <_tree.h>

namespace upan {

template <typename K, typename V>
class _map_type
{
  public:
    typedef pair<const K, V> value_type;
    typedef const K key_type;
    struct _key_accessor
    {
      const K& operator()(const value_type& e) const { return e.first; }
    };
};

template <typename K, typename V>
class map : public _tree<_map_type<K, V>>
{
  public:
    typedef _tree<_map_type<K, V>> _parent_;
    typedef typename _parent_::node node;
    typedef typename _parent_::value_type value_type;

    V& operator[](const K& key)
    {
      return this->insert(value_type(key, V())).first->second;
    }
};
}

#endif
