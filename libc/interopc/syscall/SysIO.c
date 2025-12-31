/*
 *  Mother Operating System - An x86 based Operating System
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

#include <syscalldefs.h>

int SysIO_Close(int fd) {
  return _upanix_syscall(SYS_CALL_IO_CLOSE, (uint64_t)fd, 2, 3, 4, 5);
}

int SysIO_Ctl(int fd, uint64_t cmd, uint64_t arg) {
  return _upanix_syscall(SYS_CALL_IO_CTL, (uint64_t)fd, cmd, arg, 4 ,5);
}

int SysIO_OpenPT(int flag) {
  return _upanix_syscall(SYS_CALL_IO_OPENPT, (uint64_t)flag, 2, 3, 4 ,5);
}

int SysIO_GetPTSName(int fd, char* name, int len) {
  return _upanix_syscall(SYS_CALL_IO_PTS_NAME, (uint64_t)fd, (uint64_t)name, (uint64_t)len, 4 ,5);
}

int SysIO_TCGetAttr(int fd, struct termios *termios_p) {
  return _upanix_syscall(SYS_CALL_IO_TC_GET_ATTR, (uint64_t)fd, (uint64_t)termios_p, 3, 4 ,5);
}

int SysIO_TCSetAttr(int fd, termios_actions action, const struct termios *termios_p) {
  return _upanix_syscall(SYS_CALL_IO_TC_SET_ATTR, (uint64_t)fd, (uint64_t)action, (uint64_t)termios_p, 4 ,5);
}

int SysIO_IsTTY(int fd) {
  return _upanix_syscall(SYS_CALL_IO_IS_TTY, (uint64_t)fd, 2, 3, 4 ,5);
}