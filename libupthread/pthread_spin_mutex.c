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
#include <mosstd.h>
#include <errno_values.h>

int pthread_spin_init(pthread_spinlock_t* mtx, const pthread_mutexattr_t* attr) {
  if (!mtx) return 1;
  mtx->_kind = attr ? attr->_kind : PTHREAD_MUTEX_DEFAULT;
  mtx->_lock_count = 0;
  atomic_set(&mtx->_lock, FREE_MUTEX);
  return 0;
}

void pthread_spin_destroy(pthread_spinlock_t* mtx) {
  pthread_spin_init(mtx, NULL);
}

int pthread_spin_lock(pthread_spinlock_t* mtx) {
  if (!mtx) return 1;

  const int newVal = getlockownerid();
  while (true) {
    const int oldVal = (int)atomic_compare_swap(&mtx->_lock, FREE_MUTEX, newVal);
    if (oldVal == FREE_MUTEX) {
      mtx->_lock_count = 1;
      return 0;
    } else if (oldVal == newVal) {
      ++mtx->_lock_count;
      return 0;
    } else {
      while (atomic_get(&mtx->_lock) != FREE_MUTEX) {
        __asm__ __volatile__("pause");
      }
    }
  }
}

int pthread_spin_trylock(pthread_spinlock_t* mtx) {
  if (!mtx) return 1;

  const int newVal = getlockownerid();
  const int oldVal = (int)atomic_compare_swap(&mtx->_lock, FREE_MUTEX, newVal);
  if (oldVal == FREE_MUTEX) {
    mtx->_lock_count = 1;
    return 0;
  } else if (oldVal == newVal) {
    ++mtx->_lock_count;
    return 0;
  } else {
    return EBUSY;
  }
}

int pthread_spin_unlock(pthread_spinlock_t* mtx) {
  if (!mtx) return 1;

  const int val = getlockownerid();
  if (atomic_get(&mtx->_lock) != val) {
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

int pthread_spin_unlock_owner(pthread_spinlock_t* mtx, int owner) {
  if (!mtx) return 1;

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