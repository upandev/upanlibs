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

#ifndef _PTHREAD_H_
#define _PTHREAD_H_

#include <time.h>
#include <stdlib.h>

#define PTHREAD_ONCE_INIT	0

#define PTHREAD_MUTEX_INITIALIZER	{ PTHREAD_MUTEX_NORMAL, 0 }
#define PTHREAD_COND_INITIALIZER	{ 0 }

#if defined (__cplusplus)
extern "C" {
#endif

typedef uint32_t pthread_key_t;
typedef uint32_t pthread_once_t;
typedef unsigned long int pthread_t;

typedef struct {
  int _kind;
  void* _impl_mutex;
} pthread_mutex_t;

typedef struct {
  void* _impl_cv;
} pthread_cond_t;

typedef struct {
  uint32_t _attr;
} pthread_condattr_t;

typedef enum {
 PTHREAD_MUTEX_NORMAL,
 PTHREAD_MUTEX_ERRORCHECK,
 PTHREAD_MUTEX_RECURSIVE,
 PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL,
} pthread_attr_kind;

typedef struct {
//  unsigned long   mattr_flags;
  int             _kind;
//  int             mattr_spare1;
//  int             mattr_prioceiling;
//  int             mattr_protocol;
//  int             mattr_spares2[6];
//  char            mattr_name[31+1];
} pthread_mutexattr_t;

#define PTHREAD_CREATE_JOINABLE  0
#define PTHREAD_CREATE_DETACHED  1

typedef struct {
  size_t _stack_size;
  int _detach_state;  // 0 = joinable, 1 = detached
} pthread_attr_t;

void pthread_mutexattr_init(pthread_mutexattr_t*);
int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int kind);
void pthread_mutexattr_destroy(pthread_mutexattr_t*);
int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr);
void pthread_mutex_destroy(pthread_mutex_t*);
int pthread_mutex_lock(pthread_mutex_t *);
int pthread_mutex_trylock(pthread_mutex_t *);
int pthread_mutex_unlock(pthread_mutex_t *);
void pthread_attr_init(pthread_attr_t*);
void pthread_attr_destroy(pthread_attr_t*);
int pthread_attr_setdetachstate(pthread_attr_t*, int detach_state);
void pthread_exit(void* ret);
int pthread_join(pthread_t tid, void** ret);
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void* (*start_routine)(void *), void *arg);
int pthread_equal(pthread_t, pthread_t);
int pthread_self();

int sched_yield();
int pthread_once(pthread_once_t *, void (*init)());

int pthread_key_create(pthread_key_t *, void (*destr_func) (void *));
void* pthread_getspecific(pthread_key_t);
int pthread_setspecific(pthread_key_t, const void *);
int pthread_key_delete(pthread_key_t key);

int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t* attr);
int pthread_cond_wait(pthread_cond_t*, pthread_mutex_t*);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *time);
int pthread_cond_signal(pthread_cond_t*);
int pthread_cond_broadcast(pthread_cond_t*);
int pthread_cond_destroy(pthread_cond_t*);

#if defined (__cplusplus)
}
#endif

#endif
