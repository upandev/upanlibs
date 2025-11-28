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
#include <mutex.h>
#include <map.h>
#include <mosstd.h>

class thread_manager {
private:
  thread_manager() {}

public:
  static thread_manager& instance() {
    static thread_manager tm;
    return tm;
  }

  void add(pthread_t tid, void* data) {
    upan::mutex_guard g(_m);
    _threadDataMap[tid] = data;
  }

  void* consume(pthread_t tid) {
    upan::mutex_guard g(_m);
    auto i = _threadDataMap.find(tid);
    if (i == _threadDataMap.end()) return nullptr;
    return i->second;
  }

private:
  typedef upan::map<pthread_t, void*> ThreadDataMap;
  ThreadDataMap _threadDataMap;
  upan::mutex _m;
};

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

void pthread_start_routine_wrapper(void* (*start_routine)(void *), void *arg) {
  start_routine(arg);
}

void pthread_exit(void* ret) {
  thread_manager::instance().add(getpid(), ret);
  exit(0);
}

int pthread_join(pthread_t tid, void** ret) {
  int es;
  if (waitpid(tid, &es, 0)) {
    return -1;
  }
  *ret = thread_manager::instance().consume(tid);
  return es;
}

static void upthread_entry_caller(thread_entry_func_with_ret_t tmain, void* arg) {
  void* re = tmain(arg);
  pthread_exit(re);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void* (*start_routine)(void *), void *arg) {
  bool joinable = attr ? attr->_detach_state == PTHREAD_CREATE_JOINABLE : true;
  *thread = (pthread_t)exectp(upthread_entry_caller, start_routine, arg, joinable);
  return 0;
}