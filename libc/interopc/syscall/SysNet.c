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

sock_t SysNet_CreateSocket(SA_FAMILY_TYPE sa_family, SOCKET_TYPE socket_type, int protocol) {
  return (sock_t)_upanix_syscall(SYS_CALL_SOCKET_CREATE, (uint64_t)sa_family, (uint64_t)socket_type, (uint64_t)protocol, 4, 5);
}

int SysNet_Bind(sock_t fd, const struct sockaddr* client_addr, socklen_t len) {
  return (int)_upanix_syscall(SYS_CALL_SOCKET_BIND, (uint64_t)fd, (uint64_t)client_addr, (uint64_t)len, 4, 5);
}

int SysNet_SetSockOpt(sock_t fd, int level, SOCKET_OPTION option, const void* optval, socklen_t len) {
  return (int)_upanix_syscall(SYS_CALL_SOCKET_SET_OPT, (uint64_t)fd, (uint64_t)level, (uint64_t)option, (uint64_t)optval, (uint64_t)len);
}

int SysNet_GetSockOpt(sock_t fd, int level, SOCKET_OPTION option, void* optval, socklen_t* len) {
  return (int)_upanix_syscall(SYS_CALL_SOCKET_GET_OPT, (uint64_t)fd, (uint64_t)level, (uint64_t)option, (uint64_t)optval, (uint64_t)len);
}

int SysNet_SendTo(int fd, const void *buf, size_t n, int flags, const struct sockaddr* addr, socklen_t len) {
  uint64_t ext_param[] = { (uint64_t)addr, (uint64_t)len };
  return (int)_upanix_syscall(SYS_CALL_SOCKET_SEND_TO, (uint64_t)fd, (uint64_t)buf, (uint64_t)n, (uint64_t)flags, (uint64_t)&ext_param);
}

int SysNet_RecvFrom(int fd, void *buf, size_t n, int flags, struct sockaddr* addr, socklen_t* len) {
  uint64_t ext_param[] = { (uint64_t)addr, (uint64_t)len };
  return (int)_upanix_syscall(SYS_CALL_SOCKET_RECV_FROM, (uint64_t)fd, (uint64_t)buf, (uint64_t)n, (uint64_t)flags, (uint64_t)&ext_param);
}

int SysNet_Connect(int fd, const struct sockaddr *addr, socklen_t len) {
  return (int)_upanix_syscall(SYS_CALL_SOCKET_CONNECT, (uint64_t)fd, (uint64_t)addr, (uint64_t)len, 4, 5);
}

int SysNet_Listen(int fd, int backlog) {
  return (int)_upanix_syscall(SYS_CALL_SOCKET_LISTEN, (uint64_t)fd, (uint64_t)backlog, 3, 4, 5);
}

int SysNet_Accept(int fd, struct sockaddr* addr, socklen_t* len) {
  return (int)_upanix_syscall(SYS_CALL_SOCKET_ACCEPT, (uint64_t)fd, (uint64_t)addr, (uint64_t)len, 4, 5);
}

int SysNet_Shutdown(int fd, SOCKET_SHUTDOWN_TYPE type) {
  return (int)_upanix_syscall(SYS_CALL_SOCKET_SHUTDOWN, (uint64_t)fd, (uint64_t)type, 3, 4, 5);
}

int SysNet_GetHostByName(const char* name, struct hostent** hostinfo) {
  return (int)_upanix_syscall(SYS_CALL_GET_HOST_BY_NAME, (uint64_t)name, (uint64_t)hostinfo, 3, 4, 5);
}

int SysNet_GetHostByAddr(const void* addr, socklen_t len, int type, struct hostent** hostinfo) {
  return (int)_upanix_syscall(SYS_CALL_GET_HOST_BY_ADDR, (uint64_t)addr, (uint64_t)len, (uint64_t)type, (uint64_t)hostinfo, 5);
}

void SysNet_FreeHostInfo(struct hostent* hostinfo) {
  _upanix_syscall(SYS_CALL_FREE_HOST_INFO, (uint64_t)hostinfo, 2, 3, 4, 5);
}

int SysNet_SocketPair(SA_FAMILY_TYPE domain, SOCKET_TYPE type, int protocol, int sv[2]) {
  return (int)_upanix_syscall(SYS_CALL_SOCKET_PAIR, (uint64_t)domain, (uint64_t)type, (uint64_t)protocol, (uint64_t)sv, 5);
}