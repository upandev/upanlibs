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

#pragma once

#ifdef __LOCAL_TEST__
#include "/usr/include/string.h"
#else
#include <newalloc.h>
#endif
#include <exception.h>
#include <algorithm.h>
#include <std_stuff.h>

namespace upan {

template <typename T>
class vector
{
  public:
    class vector_iterator;
    typedef vector_iterator iterator;
    typedef const vector_iterator const_iterator;
    typedef T element_type;

    vector();
    explicit vector(const int reserve);
    vector(const vector<T>& rhs);
    vector(vector<T>&& rhs);
    vector(std::initializer_list<T> l);
    ~vector();

    vector& operator=(const vector& rhs);
    vector& operator=(vector&& rhs);

    void insert(const int index, const T& v);
    void insert(const_iterator pos, const T& v);
    void push_back(const T& value);

    iterator erase(const T& value);
    iterator erase(const_iterator& it);
    iterator erase(const_iterator& first, const_iterator& last);
    iterator erase(const int first, const int last);
    void pop_back();

    int size() const { return _size; }
    bool empty() const { return _size == 0; }

    iterator begin() const;
    iterator end() const;

    void clear();
    void swap(vector& other);
    T& operator[](int index) const;

  public:
    class vector_iterator
    {
      private:
        vector_iterator(const vector<T>& parent, const int index) : _parent(&parent), _index(index)
        {
        }
      public:
        typedef T value_type;

        T& operator*() { return const_cast<T&>(value()); }
        const T& operator*() const { return value(); }

        T& operator->() { return const_cast<T&>(value()); }
        const T& operator->() const { return value(); }

        vector_iterator& operator++() { return const_cast<vector_iterator&>(pre_inc()); }
        const vector_iterator& operator++() const { return pre_inc(); }

        vector_iterator operator++(int) { return post_inc(); }
        vector_iterator operator++(int) const { return post_inc(); }

        vector_iterator& operator--() { return const_cast<vector_iterator&>(pre_dec()); }
        const vector_iterator& operator--() const { return pre_dec(); }

        vector_iterator operator--(int) { return post_dec(); }
        vector_iterator operator--(int) const { return post_dec(); }

        vector_iterator operator+(const int delta) const
        {
          const int newIndex = _index + delta;
          validate_index(newIndex);
          return vector_iterator(*_parent, newIndex);
        }

        vector_iterator operator-(const int delta) const
        {
          const int newIndex = _index - delta;
          validate_index(newIndex);
          return vector_iterator(*_parent, newIndex);
        }

        bool operator==(const vector_iterator& rhs) const { return _index == rhs._index; }
        bool operator!=(const vector_iterator& rhs) const { return !operator==(rhs); }
        bool operator<(const vector_iterator& rhs) const { return _index < rhs._index; }
        bool operator>(const vector_iterator& rhs) const { return _index > rhs._index; }

        vector_iterator& operator=(const vector_iterator& rhs)
        {
          _index = rhs._index;
          _parent = rhs._parent;
          return *this;
        }

      private:
        void validate_index(const int index) const
        {
          if(index < 0 || index > _parent->size())
            throw upan::exception(XLOC, "vector index %d out of range", index);
        }
        const T& value() const
        {
          if(_index == _parent->size())
            throw upan::exception(XLOC, "accessing end iterator on vector");
          return (*_parent)[_index];
        }
        const vector_iterator& pre_inc() const
        {
          const int newIndex = _index + 1;
          validate_index(newIndex);
          _index = newIndex;
          return *this;
        }
        vector_iterator post_inc() const
        {
          vector_iterator tmp(*_parent, _index);
          pre_inc();
          return tmp;
        }
        const vector_iterator& pre_dec() const
        {
          const int newIndex = _index - 1;
          validate_index(newIndex);
          _index = newIndex;
          return *this;
        }
        vector_iterator post_dec() const
        {
          vector_iterator tmp(*_parent, _index);
          pre_dec();
          return tmp;
        }
      private:
        const vector<T>* _parent;
        mutable int _index;
        friend class vector<T>;
    };
    friend class vector_iterator;

#ifdef __LOCAL_TEST__
  public:
#else
  private:
#endif
    void destroy();
    void expandIfNeeded();

    int    _size;
    int    _capacity;
    char*  _buffer;
};

template <typename T>
vector<T>::vector() : _size(0), _capacity(0), _buffer(nullptr) {
}

template <typename T>
vector<T>::vector(const int reserve) : vector() {
  if (reserve < 0) {
    throw upan::exception(XLOC, "invalid vector reserve size: %d - must be > 0", reserve);
  } else if (reserve > 0) {
    _capacity = sizeof(T) * reserve;
    _buffer = new char[_capacity];
  }
}

template <typename T>
vector<T>::vector(const vector<T>& rhs) : _size(0), _capacity(rhs._capacity), _buffer(nullptr)
{
  if (_capacity == 0) {
    return;
  }

  _buffer = new char[_capacity];

  try {
    for ( ; _size < rhs._size; ++_size) {
      new (_buffer + _size * sizeof(T)) T(rhs[_size]);
    }
  } catch (...) {
    clear();
    delete[] _buffer;
    throw;
  }
}

template <typename T>
vector<T>::vector(vector<T> &&rhs) : _size(rhs._size), _capacity(rhs._capacity), _buffer(rhs._buffer)
{
  rhs._size = 0;
  rhs._capacity = 0;
  rhs._buffer = nullptr;
}

//TODO: not working - needs to be fixed
template <typename T>
vector<T>::vector(std::initializer_list<T> l) : vector() {
  for(const auto& e : l) {
    this->push_back(e);
  }
}

template <typename T>
void vector<T>::destroy() {
  clear();
  delete[] _buffer;
}

template <typename T>
vector<T>::~vector() {
  destroy();
}

template <typename T>
vector<T>& vector<T>::operator=(const vector<T>& rhs)
{
  vector<T> temp(rhs);
  this->swap(temp);
  return *this;
}

template <typename T>
vector<T>& vector<T>::operator=(vector<T>&& rhs)
{
  destroy();

  _size = rhs._size;
  _capacity = rhs._capacity;
  _buffer = rhs._buffer;

  rhs._size = 0;
  rhs._capacity = 0;
  rhs._buffer = nullptr;

  return *this;
}

template <typename T>
void vector<T>::swap(vector<T>& other)
{
  int tSize = other._size;
  other._size = this->_size;
  this->_size = tSize;

  int tCapacity = other._capacity;
  other._capacity = this->_capacity;
  this->_capacity = tCapacity;

  char* tBuffer = other._buffer;
  other._buffer = this->_buffer;
  this->_buffer = tBuffer;
}

template <typename T>
void vector<T>::expandIfNeeded()
{
  if((_size * (int)sizeof(T)) >= _capacity)
  {
    const uint32_t newCapacity = sizeof(T) * (int)(_size <= 1 ? 2 : _size * 1.5);
    char* tempBuffer = new char[newCapacity];
    int i = 0;
    try {
      for (; i < _size; ++i) {
        new(tempBuffer + i * sizeof(T)) T(reinterpret_cast<T*>(_buffer)[i]);
      }
    } catch (...) {
      while (i > 0) {
        --i;
        reinterpret_cast<T*>(tempBuffer)[i].~T();
      }
      delete[] tempBuffer;
      throw;
    }
    int size = _size;
    destroy();
    _size = size;
    _buffer = tempBuffer;
    _capacity = newCapacity;
  }
}

template <typename T>
void vector<T>::insert(const int index, const T& v)
{
  T saved(v);
  expandIfNeeded();

  if(index < 0 || index > _size)
    throw upan::exception(XLOC, "invalid index %d for insert", index);

  if (index == _size) {
    new (_buffer + index * sizeof(T))T(saved);
    ++_size;
    return;
  }

  // Construct the extra tail element in uninitialized storage.
  new (_buffer + _size * sizeof(T)) T(reinterpret_cast<T*>(_buffer)[_size - 1]);

  ++_size;

  for (int i = _size - 2; i > index; --i) {
    reinterpret_cast<T*>(_buffer)[i] = reinterpret_cast<T*>(_buffer)[i - 1];
  }

  reinterpret_cast<T*>(_buffer)[index] = saved;
}

template <typename T>
void vector<T>::insert(iterator pos, const T& v)
{
  insert(pos._index, v);
}

template <typename T>
void vector<T>::push_back(const T& value)
{
  insert(_size, value);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(vector<T>::const_iterator& it)
{
  return erase(it, it + 1);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(vector<T>::const_iterator& first, vector<T>::const_iterator& last)
{
  return erase(first._index, last._index);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const int first, const int last)
{
  if (first < 0 || last < first || last > _size)
    throw upan::exception(XLOC, "vector index first:%d and last:%d out of bounds while deleting elements", first, last);

  const int count = last - first;
  if (count == 0) {
    return vector_iterator(*this, first);
  }

  for(int i = last; i < _size; ++i) {
    reinterpret_cast<T*>(_buffer)[i - count] = reinterpret_cast<T*>(_buffer)[i];
  }

  const int newSize = _size - count;

  for(int i = _size; i > newSize; --i) {
    reinterpret_cast<T*>(_buffer)[i - 1].~T();
  }

  _size = newSize;
  return vector_iterator(*this, first);
}

template <typename T>
typename vector<T>::iterator vector<T>::erase(const T& v)
{
  return erase(find(begin(), end(), v));
}

template <typename T>
void vector<T>::pop_back()
{
  erase(end() - 1);
}

template <typename T>
typename vector<T>::iterator vector<T>::begin() const
{
  return vector<T>::iterator(*this, 0);
}

template <typename T>
typename vector<T>::iterator vector<T>::end() const
{
  return vector<T>::iterator(*this, _size);
}

template <typename T>
void vector<T>::clear()
{
  for(int i = 0; i < _size; ++i)
    reinterpret_cast<T*>(_buffer)[i].~T();
  _size = 0;
}

template <typename T>
T& vector<T>::operator[](int index) const
{
  if(index < 0 || index >= _size)
    throw upan::exception(XLOC, "vector index %d is out-of-bounds", index);
  return reinterpret_cast<T*>(_buffer)[index];
}

};
