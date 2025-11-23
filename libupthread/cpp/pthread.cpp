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
#include <mutex.h>

int pthread_mutex_init(pthread_mutex_t* mtx, const pthread_mutexattr_t* attr) {
  mtx->_kind = attr->_kind;
  mtx->_impl_mutex = new upan::mutex();
}

void pthread_mutex_destroy(pthread_mutex_t* mtx) {
  delete (upan::mutex*)mtx->_impl_mutex;
}
int pthread_mutex_lock(pthread_mutex_t* mtx) {
  ((upan::mutex*)(mtx->_impl_mutex))->lock();
  return 0;
}

int pthread_mutex_unlock(pthread_mutex_t* mtx) {
  ((upan::mutex*)(mtx->_impl_mutex))->unlock();
  return 0;
}
