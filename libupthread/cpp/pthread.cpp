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
#include <map.h>
#include <mosstd.h>
#include <condition_variable.h>
#include <errno_values.h>

#define MAX_THREAD_LOCAL_KEYS 128

class thread_manager {
private:
  thread_manager();
  static thread_manager _tm;

public:
  static thread_manager& instance() {
    return _tm;
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

  bool create_key(pthread_key_t& key) {
    upan::mutex_guard g(_m);
    for(int i = 0; i < MAX_THREAD_LOCAL_KEYS; ++i) {
      if (!_threadKeys[i]) {
        _threadKeys[i] = true;
        key = i;
        return true;
      }
    }
    return false;
  }

  void delete_key(pthread_key_t key) {
    upan::mutex_guard g(_m);
    if (key < MAX_THREAD_LOCAL_KEYS) {
      _threadKeys[key] = false;
    }
  }

private:
  typedef upan::map<pthread_t, void*> ThreadDataMap;
  ThreadDataMap _threadDataMap;
  bool _threadKeys[MAX_THREAD_LOCAL_KEYS];
  upan::mutex _m;
};

struct thread_local_key_value {
  void (*_destructor)(void*);
  void* _value;
};

__thread thread_local_key_value _thread_local_data[MAX_THREAD_LOCAL_KEYS];
thread_manager thread_manager::_tm;

thread_manager::thread_manager() {
  for(auto& i : _threadKeys) i = false;
}

int pthread_mutex_init(pthread_mutex_t* mtx, const pthread_mutexattr_t* attr) {
  if (!mtx) return 1;
  mtx->_kind = attr ? attr->_kind : PTHREAD_MUTEX_DEFAULT;
  mtx->_impl_mutex = new upan::mutex();
  return 0;
}

void pthread_mutex_destroy(pthread_mutex_t* mtx) {
  delete (upan::mutex*)mtx->_impl_mutex;
  mtx->_impl_mutex = nullptr;
}

int pthread_mutex_lock(pthread_mutex_t* mtx) {
  ((upan::mutex*)(mtx->_impl_mutex))->lock();
  return 0;
}

int pthread_mutex_trylock(pthread_mutex_t* mtx) {
  return ((upan::mutex*)(mtx->_impl_mutex))->trylock();
}

int pthread_mutex_unlock(pthread_mutex_t* mtx) {
  ((upan::mutex*)(mtx->_impl_mutex))->unlock();
  return 0;
}

static void upthread_entry_caller(thread_entry_func_with_ret_t tmain, void* arg) {
  for(auto& i : _thread_local_data) {
    i._destructor = nullptr;
    i._value = nullptr;
  }
  void* re = tmain(arg);
  pthread_exit(re);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void* (*start_routine)(void *), void *arg) {
  bool joinable = attr ? attr->_detach_state == PTHREAD_CREATE_JOINABLE : true;
  *thread = (pthread_t)exectp(upthread_entry_caller, start_routine, arg, joinable);
  return 0;
}

void pthread_exit(void* ret) {
  for (int i = 0; i < MAX_THREAD_LOCAL_KEYS; ++i) {
    if (_thread_local_data[i]._destructor) {
      _thread_local_data[i]._destructor(_thread_local_data[i]._value);
    }
  }
  thread_manager::instance().add(getpid(), ret);
  exit(0);
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) {
  if (!key) return 1;
  if (thread_manager::instance().create_key(*key)) {
    _thread_local_data[*key]._destructor = destructor;
    _thread_local_data[*key]._value = nullptr;
    return 0;
  }
  return EAGAIN;
}

int pthread_setspecific(pthread_key_t key, const void* data) {
  if (key >= MAX_THREAD_LOCAL_KEYS) return EINVAL;
  _thread_local_data[key]._value = (void*)data;
  return 0;
}

void* pthread_getspecific(pthread_key_t key) {
  if (key >= MAX_THREAD_LOCAL_KEYS) return nullptr;
  return _thread_local_data[key]._value;
}

int pthread_key_delete(pthread_key_t key) {
  if (key >= MAX_THREAD_LOCAL_KEYS) return EINVAL;
  thread_manager::instance().delete_key(key);
  return 0;
}

int pthread_join(pthread_t tid, void** ret) {
  int es;
  if (waitpid(tid, &es, 0)) {
    return -1;
  }
  *ret = thread_manager::instance().consume(tid);
  return es;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
  if (!cond) {
    return 1;
  }
  cond->_impl_cv = new upan::condition_variable();
  return 0;
}

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mtx) {
  if (!cond || !mtx) return 1;
  auto cv = (upan::condition_variable*)cond->_impl_cv;
  auto m = (upan::mutex*)mtx->_impl_mutex;
  try {
    cv->wait(*m);
  } catch(...) {
    return 1;
  }
  return 0;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *time) {
  if (!cond || !mutex) return 1;
  auto cv = (upan::condition_variable*)cond->_impl_cv;
  auto m = (upan::mutex*)mutex->_impl_mutex;
  struct timeval tv;
  tv.tv_sec = time->tv_sec;
  tv.tv_usec = time->tv_nsec / 1000;

  try {
    cv->wait(*m, &tv);
  } catch(...) {
    return 1;
  }
  return 0;
}

int pthread_cond_signal(pthread_cond_t* cond) {
  if (!cond) return 1;
  auto cv = (upan::condition_variable*)cond->_impl_cv;
  cv->notify_one();
  return 0;
}

int pthread_cond_broadcast(pthread_cond_t* cond) {
  if (!cond) return 1;
  auto cv = (upan::condition_variable*)cond->_impl_cv;
  cv->notify_all();
  return 0;
}

int pthread_cond_destroy(pthread_cond_t* cond) {
  if (!cond) return 1;
  //ideally, the condition variable should return EBUSY if there are active waiters on this condition variable
  //but in this particular implementation, the onus is on the application/caller to ensure it's safe to destroy condition variable
  delete (upan::condition_variable*)cond->_impl_cv;
  return 0;
}

int pthread_once(pthread_once_t* once, void (*init)(void)) {
  if (!once || !init) return EINVAL;

  auto r = upan::atomic::op::compare_swap(*once, 0, 1);
  if (r == 0) {
    init();
    upan::atomic::op::swap(*once, 2);
  } else if (r == 1) {
    while (upan::atomic::op::add(*once, 0) != 2) {
      yield();
    }
  }

  return 0;
}