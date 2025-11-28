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
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <mosstd.h>

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

void pthread_attr_init(pthread_attr_t* attr) {
  attr->_stack_size = 0;
  attr->_detach_state = PTHREAD_CREATE_JOINABLE;
}

void pthread_attr_destroy(pthread_attr_t* attr) {
}

int pthread_attr_setdetachstate(pthread_attr_t* attr, int detach_state) {
  if (attr) {
    if (detach_state == PTHREAD_CREATE_JOINABLE && detach_state == PTHREAD_CREATE_DETACHED) {
      attr->_detach_state = detach_state;
      return 0;
    }
  }
  return -1;
}

int sched_yield()
{
	return 0;
}

static void* threadDataTable[64]; 
static int freeEntry = 0;

int pthread_key_create(pthread_key_t* key, void (*f)(void*)) 
{
	assert(freeEntry < 64);
 
	*key = freeEntry;
	freeEntry++;
	return 0;
}
 
int pthread_once(pthread_once_t* control, void (*init)(void)) 
{
	if (*control == 0) {
		(*init)();
		*control = 1;
	}
	return 0;
}
 
void* pthread_getspecific(pthread_key_t key) 
{
	return threadDataTable[key];
}
 
int pthread_setspecific(pthread_key_t key, const void* data) 
{
	threadDataTable[key] = (void*)data;
	return 0;
}

int pthread_cond_wait(pthread_cond_t* c, pthread_mutex_t* m) 
{
	return 0;
}
 
int pthread_cond_signal(pthread_cond_t* c) 
{
	return 0;
}

int pthread_equal(pthread_t t1, pthread_t t2) {
	return t1 == t2;
}

int pthread_self() {
  return getpid();
}