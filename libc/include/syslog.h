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
#ifndef _SYSLOG_H_
#define _SYSLOG_H_

#include <stdarg.h>

#if defined __cplusplus
extern "C" {
#endif

#define LOG_MASK(pri)   (1 << (pri))
#define LOG_UPTO(pri)   ((1 << ((pri) + 1)) - 1)
#define MAX_LOG_MESSAGE_SIZE 2048
extern const char SYS_LOG_PATH[20];

typedef enum {
  LOG_EMERG,    // 0: System is unusable
  LOG_ALERT,    // 1: Action must be taken immediately
  LOG_CRIT,     // 2: Critical conditions
  LOG_ERR,      // 3: Error conditions
  LOG_WARNING,  // 4: Warning conditions
  LOG_NOTICE,   // 5: Normal but significant condition
  LOG_INFO,     // 6: Informational
  LOG_DEBUG,    // 7: Debug-level messages
  LOG_PRIORITY_COUNT
} LOG_PRIORITY;

typedef enum {
  LOG_PID = 1,
  LOG_CONS = 2
} LOG_FLAGS;

typedef enum {
  LOG_KERN,     // Kernel messages
  LOG_USER,     // Default if no facility is specified
  LOG_SYSLOG,   // Messages from syslogd itself
} LOG_CATEGORY;

void openlog(const char *ident, int option, int facility);
void syslog(int priority, const char *format, ...);
void closelog();
int setlogmask(int priority_mask);

int enable_log_priority(int priority);
int disable_log_priority(int priority);
int str_to_log_priority(const char* priority);
const char* log_priority_to_str(int priority);

void syslog_arg(int priority, const char *fmsg, va_list arg);
void construct_log_msg(char *buf, int buflen, int priority, const char* ident, int option, int facility, const char *fmsg, va_list arg);

#if defined __cplusplus
}
#endif

#endif
