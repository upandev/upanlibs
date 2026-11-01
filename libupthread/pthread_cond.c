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
#include <string.h>
#include <mosstd.h>
#include <atomicop.h>

typedef struct {
  _Alignas(4) uint32_t _val;
} global_cv_id_seq_type;

global_cv_id_seq_type _global_cv_id_seq = { 1 };

static int get_cond_id(pthread_cond_t* cond) {
  uint32_t id = atomic_get(&cond->_id);

  if (id == 0) {
    uint32_t val = atomic_add(&_global_cv_id_seq._val, 1);
    uint32_t prev = atomic_compare_swap(&cond->_id, 0, val);
    id = prev == 0 ? val : prev;
  }

  return (int)id;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
  if (!cond) {
    return 1;
  }
  atomic_set(&cond->_id, atomic_add(&_global_cv_id_seq._val, 1));
  return 0;
}

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mtx) {
  if (!cond || !mtx) return 1;
  int ret = waitqueue(get_cond_id(cond), mtx, NULL);
  pthread_mutex_lock(mtx);
  return ret;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mtx, const struct timespec *abstime) {
  if (!cond || !mtx) return 1;
  int ret = waitqueue(get_cond_id(cond), mtx, abstime);
  pthread_mutex_lock(mtx);
  return ret;
}

int pthread_cond_signal(pthread_cond_t* cond) {
  if (!cond) return 1;
  waitdequeue(get_cond_id(cond), false);
  return 0;
}

int pthread_cond_broadcast(pthread_cond_t* cond) {
  if (!cond) return 1;
  waitdequeue(get_cond_id(cond), true);
  return 0;
}

int pthread_cond_destroy(pthread_cond_t* cond) {
  if (!cond) return 1;
  return 0;
}