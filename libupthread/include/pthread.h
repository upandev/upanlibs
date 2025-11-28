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

#include <stdlib.h>

#define PTHREAD_ONCE_INIT	0

#define __LOCK_INITIALIZER	{ 0, 0 }

#define PTHREAD_MUTEX_INITIALIZER	{ PTHREAD_MUTEX_NORMAL }
#define PTHREAD_COND_INITIALIZER	{__LOCK_INITIALIZER,0}

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct _pthread_descr_struct *_pthread_descr;
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;
typedef unsigned long int pthread_t;
typedef long long int __pthread_cond_align_t;

struct _pthread_fastlock {
    long int __status;
    int __spinlock;
};

typedef struct {
  int _kind;
  void* _impl_mutex;
} pthread_mutex_t;

typedef struct {
    struct _pthread_fastlock __c_lock;
    _pthread_descr __c_waiting;
    char __padding[48 - sizeof(struct _pthread_fastlock) -
		   sizeof(_pthread_descr) -
		   sizeof(__pthread_cond_align_t)];
    __pthread_cond_align_t __align;
} pthread_cond_t;

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
int pthread_key_create(pthread_key_t *, void (*destr_func) (void *));
void *pthread_getspecific(pthread_key_t);
int pthread_setspecific(pthread_key_t, const void *);
int pthread_once(pthread_once_t *, void (*init_routine) (void));
int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int pthread_cond_signal(pthread_cond_t *);

#if defined (__cplusplus)
}
#endif

#endif
