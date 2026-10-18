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

#include <pthread.h>
#include <atomicop.h>
#include <string.h>
#include <mosstd.h>
#include <errno_values.h>

#define FREE_MUTEX (-999)

int pthread_mutex_init(pthread_mutex_t* mtx, const pthread_mutexattr_t* attr) {
  if (!mtx) return 1;
  mtx->_kind = attr ? attr->_kind : PTHREAD_MUTEX_DEFAULT;
  mtx->_lock_count = 0;
  atomic_set(&mtx->_lock, FREE_MUTEX);
  return 0;
}

void pthread_mutex_destroy(pthread_mutex_t* mtx) {
  pthread_mutex_init(mtx, NULL);
}

int pthread_mutex_lock(pthread_mutex_t* mtx) {
  if (!mtx) return 1;
  if (iskernel()) return 0;

  const int newVal = getpid();
  const int oldVal = (int)atomic_compare_swap(&mtx->_lock, FREE_MUTEX, newVal);
  if (oldVal == FREE_MUTEX) {
    mtx->_lock_count = 1;
  } else if (oldVal == newVal) {
    ++mtx->_lock_count;
  } else {
    waitonlock((uint64_t)&mtx->_lock, FREE_MUTEX, newVal);
    mtx->_lock_count = 1;
  }
  return 0;
}

int pthread_mutex_trylock(pthread_mutex_t* mtx) {
  if (!mtx) return 1;
  if (iskernel()) return 0;

  const int newVal = getpid();
  const int oldVal = (int)atomic_compare_swap(&mtx->_lock, FREE_MUTEX, newVal);
  if (oldVal == FREE_MUTEX) {
    mtx->_lock_count = 1;
  } else if (oldVal == newVal) {
    ++mtx->_lock_count;
  } else {
    return EBUSY;
  }
  return 0;
}

int pthread_mutex_unlock(pthread_mutex_t* mtx) {
  if (!mtx) return 1;
  if (iskernel()) return 0;

  if (atomic_get(&mtx->_lock) != getpid()) {
    return 1;
  }

  if (mtx->_lock_count > 0) {
    --mtx->_lock_count;
  }

  if (!mtx->_lock_count) {
    atomic_set(&mtx->_lock, FREE_MUTEX);
  }

  return 0;
}

int pthread_mutex_unlock_owner(pthread_mutex_t* mtx, int owner) {
  if (!mtx) return 1;
  if (iskernel()) return 0;

  if (atomic_get(&mtx->_lock) != owner) {
    return 1;
  }

  if (mtx->_lock_count > 0) {
    --mtx->_lock_count;
  }

  if (!mtx->_lock_count) {
    atomic_set(&mtx->_lock, FREE_MUTEX);
  }

  return 0;
}

void pthread_mutexattr_init(pthread_mutexattr_t* attr) {
  memset(attr, 0, sizeof(pthread_mutexattr_t));
  attr->_kind = PTHREAD_MUTEX_DEFAULT;
}

int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int kind) {
  int pv = attr->_kind;
  attr->_kind = kind;
  return pv;
}

void pthread_mutexattr_destroy(pthread_mutexattr_t* attr) {
}