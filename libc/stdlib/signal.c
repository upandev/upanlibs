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

#include <signal.h>
#include <syscalldefs.h>
#include <string.h>

int sigemptyset(sigset_t *set) {
  if (!set) {
    return -1;
  }
  memset(set, 0, sizeof(*set));
  return 0;
}

int sigfillset(sigset_t *set) {
  if (!set) {
    return -1;
  }
  memset(set, 0xFF, sizeof(*set));
  return 0;
}

int sigaddset(sigset_t *set, int signo) {
  if (!set || signo <= 0 || signo >= _NSIG) {
    return -1;
  }

  unsigned int bit = signo - 1;
  set->__val[bit / _NSIG_BITS_PER_WORD] |= 1UL << (bit % _NSIG_BITS_PER_WORD);
  return 0;
}

int sigdelset(sigset_t *set, int signo) {
  if (!set || signo <= 0 || signo >= _NSIG) {
    return -1;
  }

  unsigned int bit = signo - 1;
  set->__val[bit / _NSIG_BITS_PER_WORD] &= ~(1UL << (bit % _NSIG_BITS_PER_WORD));
  return 0;
}

int sigismember(const sigset_t *set, int signo) {
  if (!set || signo <= 0 || signo >= _NSIG) {
    return -1;
  }

  unsigned int bit = signo - 1;
  return !!(set->__val[bit / _NSIG_BITS_PER_WORD] & (1UL << (bit % _NSIG_BITS_PER_WORD)));
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
  return SysProcess_MaskSignal(how, set, oldset);
}

int kill(pid_t pid, SIGNAL signo) {
  return SysProcess_SendSignal(pid, signo, NULL);
}

int sigqueue(pid_t pid, SIGNAL signo, const union sigval value) {
  return SysProcess_SendSignal(pid, signo, &value);
}

void signal_restorer() {
  SysProcess_SignalReturn();
}

int sigaction(int signo, const struct sigaction *act, struct sigaction *oldact) {
  if (act) {
    ((struct sigaction*)act)->sa_restorer = &signal_restorer;
  }
  return SysProcess_SetSignalAction(signo, act, oldact);
}

bool isignoreaction(const struct sigaction *act) {
  if (!act) return true;
  if (act->sa_flags & SA_SIGINFO && (uintptr_t)act->sa_sigaction == SIG_IGN) return true;
  if (!(act->sa_flags & SA_SIGINFO) && (uintptr_t)act->sa_handler == SIG_IGN) return true;
  return false;
}

bool isdefaultaction(const struct sigaction *act) {
  if (!act) return true;
  if (act->sa_flags & SA_SIGINFO && (uintptr_t)act->sa_sigaction == SIG_DFL) return true;
  if (!(act->sa_flags & SA_SIGINFO) && (uintptr_t)act->sa_handler == SIG_DFL) return true;
  return false;
}
