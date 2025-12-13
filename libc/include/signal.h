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
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <dtime.h>
#include <mosstd.h>

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  SIGINT = 2,
  SIGQUIT = 3,
  SIGILL = 4, /* Illegal instruction */
  SIGTRAP = 5, /* Trace/breakpoint trap */
  SIGABRT = 6, /* Abort (not asked, but near FPE) */
  SIGBUS = 7, /* Bus error (bad memory access) */
  SIGFPE = 8, /* Floating point exception */
  SIGKILL = 9,
  SIGSEGV = 11,
  SIGALARM = 14,
  SIGTERM = 15,
  SIGCHLD = 17,
  SIGCONT = 18,
  SIGSTOP = 19,
  SIGTSTP = 20,
} SIGNAL;

//required in external library/app code which does compile time checks to see if SIGALRM is defined
#define SIGALRM 14

typedef enum {
  SA_NOCLDSTOP = 1u,        /* Don't send SIGCHLD when children stop */
  SA_NOCLDWAIT = 2u,        /* Don't leave zombies */
  SA_SIGINFO = 4u,        /* Use sa_sigaction instead of sa_handler */
  SA_ONSTACK = 0x08000000u, /* Deliver on alternate signal stack */
  SA_RESTART = 0x10000000u, /* Restart some interrupted syscalls */
  SA_NODEFER = 0x40000000u, /* Don’t automatically block signal */
  SA_RESETHAND = 0x80000000u, /* Reset handler to default on entry */
} SA_FLAG;

typedef enum {
  SIG_BLOCK = 0, /* Block signals: mask |= set  */
  SIG_UNBLOCK = 1,  /* Unblock signals: mask &= ~set */
  SIG_SETMASK = 2,  /* Set mask: mask = set */
} SIG_MASKING_TYPE;

#define SIG_DFL 0
#define SIG_IGN 1

#define _NSIG 256
#define _NSIG_BITS_PER_WORD 64 //8 * sizeof(uint64_t)
#define _NSIG_WORDS (_NSIG / _NSIG_BITS_PER_WORD) // 4

typedef struct {
  uint64_t __val[_NSIG_WORDS];
} __sigset_t;

typedef __sigset_t sigset_t;
typedef int sig_atomic_t;

union sigval {
  int   sival_int;   // integer value
  void *sival_ptr;   // pointer value
};

typedef struct siginfo {
  int      si_signo;   /* Signal number */
  int      si_errno;   /* If nonzero, errno associated */
  int      si_code;    /* Signal code (cause) */
  union sigval si_value;

  union {
    //Padding to fixed size - for now, kernel, libc share same definition.
    //and we need to recompile all apps and libraries if we add more entries in the union in future
    //int _pad[...];

    /* kill() / tgkill() */
    struct {
      pid_t si_pid;  /* Sending process PID */
      uid_t si_uid;  /* Real user ID of sender */
    } _kill;

    /* Signals like SIGSEGV, SIGILL, SIGBUS */
    struct {
      void *si_addr;    /* Faulting address */
    } _sigfault;

    /* SIGCHLD */
    struct {
      pid_t si_pid;     /* Child PID */
      uid_t si_uid;     /* Child UID */
      int   si_status;  /* Exit value or signal */
      time_t si_utime;
      time_t si_stime;
    } _sigchld;

    /* SIGPOLL / SIGIO */
    struct {
      int64_t si_band;
      int  si_fd;
    } _sigpoll;
  } _sifields;
} siginfo_t;

typedef void (*sa_handler_t)(int);
typedef void (*sa_sigaction_t)(int, siginfo_t *, void *);

struct sigaction {
  sa_handler_t sa_handler;
  sa_sigaction_t sa_sigaction;
  sigset_t sa_mask;                       /* signals to block during handler */
  int      sa_flags;                      /* flags to modify behavior */
  void     (*sa_restorer)();
};

int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signo);
int sigdelset(sigset_t *set, int signo);
int sigismember(const sigset_t *set, int signo);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

int kill(pid_t pid, SIGNAL signo);
int sigqueue(pid_t pid, SIGNAL signo, union sigval value);
sa_handler_t signal(int signo, sa_handler_t handler);
int sigaction(int signo, const struct sigaction *act, struct sigaction *oldact);
bool isignoreaction(const struct sigaction *act);
bool isdefaultaction(const struct sigaction *act);
uint32_t alarm(uint32_t seconds);

#if defined __cplusplus
}
#endif

#endif
