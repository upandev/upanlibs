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

#include <syslog.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <fs.h>
#include <stdio.h>
#include <interopc.h>

#define MAX_IDENT_SIZE 128

static char log_ident[MAX_IDENT_SIZE + 1] = "";
static int log_mask = LOG_UPTO(LOG_INFO);
static int log_facility = LOG_USER;
static bool syslog_connecting = false;
static const char log_priority_strings[LOG_PRIORITY_COUNT][20] = { "EMERGENCY", "ALERT", "CRITICAL", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG" };

const char SYS_LOG_PATH[20] = "/dev/log";

static void open_syslog_socket() {
  //if it is syslog that is used within kernel then connection will be handled within kernel - because all kernel processes will share the same syslog descriptor to send the log to
  //syslog_connecting - will ensure that any syslog called while connecting - like opening socket, connect() etc. will go to console and prevent recursion of syslog() call
  if (!syslog_iskernel() && get_syslog_fd() < 0 && !syslog_connecting) {
    syslog_connecting = true;
    int sd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (sd >= 0) {
      struct sockaddr_un addr;
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_LOCAL;
      strcpy(addr.sun_path, SYS_LOG_PATH);
      if (connect(sd, (struct sockaddr*)&addr, sizeof(addr))) {
        close(sd);
      } else {
        set_syslog_fd(sd);
      }
    }
    syslog_connecting = false;
  }
}

void openlog(const char *ident, int option, int facility) {
  int is = strlen(ident);
  is = is > MAX_IDENT_SIZE ? MAX_IDENT_SIZE : is;
  memcpy(log_ident, ident, is);
  log_ident[is] = '\0';

  set_syslog_option(option);
  log_facility = facility;
  open_syslog_socket();
}

void closelog() {
  strcpy(log_ident, "");
  log_mask = LOG_UPTO(LOG_INFO);
  set_syslog_option_default();
  log_facility = LOG_USER;
  syslog_connecting = false;

  if (get_syslog_fd() >= 0) {
    close(get_syslog_fd());
    set_syslog_fd(-1);
  }
}

int setlogmask(int mask) {
  int prev = log_mask;
  log_mask = mask;
  return prev;
}

int enable_log_priority(int priority) {
  int prev = log_mask;
  log_mask |= (1 << priority);
  return prev;
}

int disable_log_priority(int priority) {
  int prev = log_mask;
  log_mask &= ~(1 << priority);
  return prev;
}

int str_to_log_priority(const char* priority) {
  for (int i = 0; i < LOG_PRIORITY_COUNT; ++i) {
    if (strcasecmp(priority, log_priority_strings[i]) == 0) {
      return i;
    }
  }
  return LOG_DEBUG;
}

const char* log_priority_to_str(int priority) {
  if (priority < 0 || priority >= LOG_PRIORITY_COUNT) {
    return log_priority_strings[LOG_DEBUG];
  }
  return log_priority_strings[priority];
}

void construct_log_msg(char *buf, int buflen, int priority, const char* ident, int option, int facility, const char *fmsg, va_list arg) {
  int n;
  int pos = 0;
  int remaining = buflen;

  n = snprintf(buf, remaining, " [%s]", log_priority_to_str(priority));
  remaining -= n;
  pos += n;
  if (remaining <= 0) {
    return;
  }

  if (ident && ident[0]) {
    n = snprintf(buf + pos, remaining, " [%s]", ident);
    remaining -= n;
    pos += n;

    if (remaining <= 0) {
      return;
    }
  }

  if (option & LOG_PID) {
    n = snprintf(buf + pos, remaining, "[%d] ", getpid());
    remaining -= n;
    pos += n;
    if (remaining <= 0) {
      return;
    }
  }

  n = snprintf(buf + pos, remaining, ": ");
  remaining -= n;
  pos += n;
  if (remaining <= 0) {
    return;
  }

  vsnprintf(buf + pos, remaining, fmsg, arg);
}

void syslog(int priority, const char *fmsg, ...) {
  va_list arg;
  va_start(arg, fmsg);
  syslog_arg(priority, fmsg, arg);
  va_end(arg);
}

void syslog_arg(int priority, const char *fmsg, va_list arg) {
  if (!(log_mask & (1 << priority))) {
    return;
  }

  open_syslog_socket();

  if (get_syslog_fd() < 0) {
    if (!(get_syslog_option() & LOG_CONS)) {
      return;
    }
  }

  char message_buffer[MAX_LOG_MESSAGE_SIZE];
  construct_log_msg(message_buffer, MAX_LOG_MESSAGE_SIZE, priority, log_ident, get_syslog_option(), log_facility, fmsg, arg);

  if (get_syslog_fd() < 0) {
    printf("\n%s", message_buffer);
  } else {
    send(get_syslog_fd(), message_buffer, strlen(message_buffer), 0);
  }
}