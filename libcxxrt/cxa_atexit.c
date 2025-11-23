/**
 * Copyright (c) 2012 David Chisnall.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include <pthread.h>
#include <stdlib.h>
#include <malloc.h>
#include <mosstd.h>

static struct atexit_handler {
  void (*destructor)(void *);
  void* arg;
  void* dso;
  struct atexit_handler* next;
} *head = NULL;

//static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int __cxa_atexit(void (*destructor)(void*), void* arg, void* dso) {
	//No need for atexit in kernel or inside thread
	if (iskernel() || isthread()) {
    return 0;
  }

  //TODO: make this thread safe!
  //pthread_mutex_lock(&lock);
  struct atexit_handler *h = malloc(sizeof(*h));
  if (!h) {
    //pthread_mutex_unlock(&lock);
    return 1;
  }
  h->destructor = destructor;
  h->arg = arg;
  h->dso = dso;
  h->next = head;
  head = h;
  //pthread_mutex_unlock(&lock);
  return 0;
}

void __cxa_finalize(void *dso) {
	//No auto-object cleanup when os is shutdown
	if (iskernel() || isthread()) {
    return;
  }
  //pthread_mutex_lock(&lock);
  struct atexit_handler** last = &head;
  for (struct atexit_handler *h = head; h; h = h->next) {
    if (h->destructor && (h->dso == dso || dso == NULL)) {
      *last = h->next;
      h->destructor(h->arg);
      free(h);
    } else {
      *last = h->next;
    }
  }
  //pthread_mutex_unlock(&lock);
}