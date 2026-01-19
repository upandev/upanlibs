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

#pragma once

#if defined __cplusplus
extern "C" {
#endif

# include <stdlib.h>
# include <stdbool.h>
# include <fs.h>
# include <mosstd.h>
# include <drive.h>
# include <cdisplay.h>
# include <sys/socket.h>
# include <netdb.h>
# include <mosstd.h>
# include <signal.h>
# include <termios.h>
# include <dirent.h>
# include <sys/resource.h>
# include <sys/select.h>

uint64_t _upanix_syscall(uint64_t sysCallId, uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t p5);

/*********************************** SYS CALL NUMBERS *******************************/

typedef enum
{
	SYS_CALL_DISPLAY_START = 0,
		SYS_CALL_DISPLAY_MESSAGE,
		SYS_CALL_DISPLAY_CLR_SCR,
		SYS_CALL_DISPLAY_MOV_CURSOR,
		SYS_CALL_DISPLAY_CLR_LINE,
		SYS_CALL_DISPLAY_SET_CURSOR,
		SYS_CALL_DISPLAY_GET_CURSOR,
		SYS_CALL_DISPLAY_RAW_CHAR,
    SYS_CALL_DISPLAY_RAW_CHAR_AREA,
    SYS_CALL_DISPLAY_CONSOLE_SIZE,
    SYS_CALL_DISPLAY_INIT_GUI_FRAME,
    SYS_CALL_DISPLAY_GUI_FRAME_TOUCH,
    SYS_CALL_DISPLAY_GUI_FRAME_HAS_ALPHA,
    SYS_CALL_DISPLAY_SET_GUI_BASE,
    SYS_CALL_DISPLAY_SET_MOUSE_CURSOR_TYPE,
    SYS_CALL_DISPLAY_INIT_TERM_CONSOLE,
    SYS_CALL_DISPLAY_INIT_GUI_EVENT_STREAM,
    SYS_CALL_DISPLAY_SET_VIEWPORT,
    SYS_CALL_DISPLAY_GET_VIEWPORT,
	SYS_CALL_DISPLAY_END,
	
	SYS_CALL_FILE_START = 200,
		SYS_CALL_CHANGE_DIR,
		SYS_CALL_CWD,
		SYS_CALL_MKDIR,
		SYS_CALL_RMDIR,
    SYS_CALL_FILE_RENAME,
    SYS_CALL_FILE_SYMLINK,
    SYS_CALL_FILE_READLINK,
    SYS_CALL_FILE_OPEN_DIR,
    SYS_CALL_FILE_READ_DIR,
    SYS_CALL_FILE_CLOSE_DIR,
		SYS_CALL_FILE_OPEN,
    SYS_CALL_FILE_OPEN_STREAM,
		SYS_CALL_FILE_CREATE,
		SYS_CALL_FILE_READ,
		SYS_CALL_FILE_WRITE,
		SYS_CALL_FILE_SELECT,
		SYS_CALL_FILE_SEEK,
		SYS_CALL_FILE_TELL,
		SYS_CALL_FILE_MODE,
		SYS_CALL_FILE_STAT,
		SYS_CALL_FILE_STAT_FD,
    SYS_CALL_FILE_SET_MODE,
		SYS_CALL_FILE_ACCESS,
    SYS_CALL_FILE_DUP,
		SYS_CALL_FILE_DUP2,
	SYS_CALL_FILE_END,

  SYS_CALL_IO_START = 300,
    SYS_CALL_IO_CLOSE,
    SYS_CALL_IO_CTL,
    SYS_CALL_IO_OPENPT,
    SYS_CALL_IO_PTS_NAME,
    SYS_CALL_IO_TC_GET_ATTR,
    SYS_CALL_IO_TC_SET_ATTR,
    SYS_CALL_IO_IS_TTY,
    SYS_CALL_IO_CREATE_PIPE,
  SYS_CALL_IO_END,

	SYS_CALL_MEM_START = 400,
    SYS_CALL_ALIGNED_ALLOC,
		SYS_CALL_FREE,
		SYS_CALL_GET_ALLOC_SIZE,
	SYS_CALL_MEM_END,

	SYS_CALL_PROC_START = 600,
    SYS_CALL_DLL_RELOCATE, //do not change this, and retain the value 601 as this is used by dynamic symbol relocation in DLL resolver system call
    SYS_CALL_PROCESS_INIT_RELOCATE,
		SYS_CALL_PROCESS_EXEC,
		SYS_CALL_THREAD_EXEC,
    SYS_CALL_THREAD_DETACH,
		SYS_CALL_PROCESS_WAIT_PID,
    SYS_CALL_PROCESS_WAIT_ON_LOCK,
    SYS_CALL_PROCESS_WAIT_QUEUE,
    SYS_CALL_PROCESS_WAIT_DEQUEUE,
		SYS_CALL_PROCESS_EXIT,
    SYS_CALL_PROCESS_YIELD,
		SYS_CALL_PROCESS_SLEEP,
		SYS_CALL_PROCESS_GET_PS_LIST,
		SYS_CALL_PROCESS_FREE_PS_LIST,
		SYS_CALL_PROCESS_CHILD_ALIVE,
		SYS_CALL_PROCESS_ALIVE,
    SYS_CALL_PROCESS_MASK_SIGNAL,
    SYS_CALL_PROCESS_SIGNAL,
    SYS_CALL_PROCESS_SET_SIGNAL_ACTION,
    SYS_CALL_PROCESS_SET_SIGNAL_RETURN,
    SYS_CALL_PROCESS_SET_SID,
    SYS_CALL_PROCESS_SET_ALARM,
	SYS_CALL_PROC_END,

	SYS_CALL_KB_START = 800,
	SYS_CALL_KB_END,

	SYS_CALL_DRIVE_START = 900,
		SYS_CALL_CHANGE_DRIVE,
		SYS_CALL_SHOW_DRIVES,
		SYS_CALL_MOUNT_DRIVE,
		SYS_CALL_UNMOUNT_DRIVE,
		SYS_CALL_FORMAT_DRIVE,
		SYS_CALL_CURRENT_DRIVE_STAT,
	SYS_CALL_DRIVE_END,

	SYS_CALL_UTIL_START = 1200,
		SYS_CALL_UTIL_DTIME,
    SYS_CALL_UTIL_BTIME,
    SYS_CALL_UTIL_TOD,
    SYS_CALL_UTIL_GET_ENTROPY,
    SYS_CALL_UTIL_GET_RUSAGE,
		SYS_CALL_UTIL_REBOOT,
	SYS_CALL_UTIL_END,

  SYS_CALL_NETWORK_START = 1300,
    SYS_CALL_SOCKET_CREATE,
    SYS_CALL_SOCKET_BIND,
    SYS_CALL_SOCKET_SET_OPT,
    SYS_CALL_SOCKET_GET_OPT,
    SYS_CALL_SOCKET_GET_NAME,
    SYS_CALL_SOCKET_PEER_NAME,
    SYS_CALL_SOCKET_SEND_TO,
    SYS_CALL_SOCKET_RECV_FROM,
    SYS_CALL_SOCKET_CONNECT,
    SYS_CALL_SOCKET_LISTEN,
    SYS_CALL_SOCKET_ACCEPT,
    SYS_CALL_SOCKET_SHUTDOWN,
    SYS_CALL_GET_HOST_BY_NAME,
    SYS_CALL_GET_HOST_BY_ADDR,
    SYS_CALL_FREE_HOST_INFO,
    SYS_CALL_SOCKET_PAIR,
  SYS_CALL_NETWORK_END,
} SYS_CALL_NUMBERS ;

/************************************************************************************/

void SysDisplay_ClearScreen();
void SysDisplay_MoveCursor(int n);
void SysDisplay_ClearLine(int pos);
void SysDisplay_SetCursor(__volatile__ int iCurPos, __volatile__ bool bUpdateCursorOnScreen);
int SysDisplay_GetCursor();
void SysDisplay_RawCharacter(__volatile__ const char ch, __volatile__ unsigned uiAttr, __volatile__ bool bUpdateCursorOnScreen);
void SysDisplay_RawCharacterArea(const MChar* src, uint32_t rows, uint32_t cols, int curPos);

int SysDrive_ChangeDrive(const char* szDriveName, char** retPwd);
int SysDrive_ShowDrives(DriveStat** pDriveList, int* iListSize);
int SysDrive_Mount(const char* szDriveName);
int SysDrive_UnMount(const char* szDriveName);
int SysDrive_Format(const char* szDriveName);
int SysDrive_GetCurrentDriveStat(DriveStat* pDriveStat);

int SysFS_ReadLink(const char *link, char *buf, size_t bufsize);
int SysFS_SymLink(const char *target, const char *link);
int SysFS_Rename(const char* oldPath, const char* newPath);
int SysFS_ChangeDirectory(const char* szDirPath, char** retPwd);
int SysFS_CreateDirectory(const char* szDirPath, unsigned short usAttribute);
int SysFS_DeleteDirectory(const char* szDirPath);
DIR* SysFS_OpenDir(const char* szDirPath);
struct dirent* SysFS_ReadDir(DIR* dirp);
int SysFS_CloseDir(DIR* dirp);
int SysFS_CreateFile(const char* szDirPath, mode_t mode);
int SysFS_FileOpen(const char* szFileName, int flags, mode_t mode);
int SysFS_FileOpenStream(uint32_t mode);
int SysFS_FileRead(int fd, void* buf, int len);
int SysFS_FileWrite(int fd, const void* buf, int len);
int SysFS_FileSeek(int fd, int offSet, int seekType);
int SysFS_FileTell(int fd);
int SysFS_FileOpenMode(int fd);
int SysFS_FileStat(const char* path, struct stat* st, bool followLink);
int SysFS_FileStatFD(int iFD, struct stat* pFileStat);
int SysFS_SetMode(const char *path, mode_t mode);
int SysFS_Dup(int oldFD);
int SysFS_Dup2(int oldFD, int newFD);
int SysFS_CWD(char* uiReturnDirPathAddress, int len);
int SysFS_FileAccess(const char* szFileName, int mode);
int SysFS_FileSelect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

int SysIO_Close(int fd);
int SysIO_Ctl(int fd, uint64_t cmd, uint64_t arg);
int SysIO_OpenPT(int flags);
int SysIO_GetPTSName(int fd, char* name, int len);

int SysMemory_AlignedAlloc(void** addr, uint32_t alignment, uint32_t uiSizeInBytes);
int SysMemory_Free(void* uiAddress);
int SysMemory_GetAllocSize(void* address, size_t* size);
process_init_fini_t* SysProcess_InitRelocate();
int SysProcess_Exec(const char* szFileName, const char *const argv[], const char *const envp[]);
int SysProcess_ThreadExec(uintptr_t threadCaller, uintptr_t entryAddress, void* arg, bool joinable);
int SysProcess_ThreadDetach(int pid);
int SysProcess_WaitPID(pid_t pid, int *status, int options);
void SysProcess_WaitOnLock(uint64_t lockAddress, int newVal, int curVal);
int SysProcess_WaitQueue(int id, void* mutex, const struct timeval* timeout);
void SysProcess_WaitDequeue(int id, bool all);
void SysProcess_Exit(int iExitStatus);
void SysProcess_Yield();
int SysProcess_Sleep(unsigned milisec);
int SysProcess_GetProcList(PS** pProcList, unsigned* uiListSize);
void SysProcess_FreeProcListMem(PS* pProcList, unsigned uiListSize);
int SysProcess_MaskSignal(SIG_MASKING_TYPE how, const sigset_t *set, sigset_t *oldset);
int SysProcess_SendSignal(pid_t pid, SIGNAL signo, const union sigval* value);
int SysProcess_SetSignalAction(int signo, const struct sigaction *act, struct sigaction *oldact);
void SysProcess_SignalReturn(void* signalContext);
int SysProcess_SetSID();
uint32_t SysProcess_SetAlarm(uint32_t seconds);

void SysUtil_GetDateTime(RTCDateTime* rtcDateTime);
void SysUtil_Reboot();

int SysUtil_GetTimeOfDay(struct timeval* pTV);
uint32_t SysUtil_GetTimeSinceBoot();
int SysUtil_GetEntropy(void *buffer, size_t length);
int SysUtil_GetResourceUsage(RUSAGE_ID who, struct rusage* ru);

int SysProcess_IsProcessAlive(int iProcessID);
int SysProcess_IsKernel();
int SysProcess_IsChildAlive(int iProcessID);

sock_t SysNet_CreateSocket(SA_FAMILY_TYPE sa_family, SOCKET_TYPE socket_type, int protocol);
int SysNet_Bind(sock_t fd, const struct sockaddr* client_addr, socklen_t len);
int SysNet_SetSockOpt(sock_t fd, int level, SOCKET_OPTION option, const void* optval, socklen_t len);
int SysNet_GetSockOpt(sock_t fd, int level, SOCKET_OPTION option, void* optval, socklen_t* len);
int SysNet_GetSockName(sock_t fd, struct sockaddr *addr, socklen_t *addrlen);
int SysNet_GetPeerName(sock_t fd, struct sockaddr *addr, socklen_t *addrlen);
int SysNet_SendTo(int fd, const void *buf, size_t n, int flags, const struct sockaddr* addr, socklen_t len);
int SysNet_RecvFrom(int fd, void *buf, size_t n, int flags, struct sockaddr* addr, socklen_t* len);
int SysNet_Connect(int fd, const struct sockaddr *addr, socklen_t len);
int SysNet_Listen(int fd, int backlog);
int SysNet_Accept(int fd, struct sockaddr* addr, socklen_t* len);
int SysNet_Shutdown(int fd, SOCKET_SHUTDOWN_TYPE type);
int SysNet_GetHostByName(const char* name, struct hostent** hostinfo);
int SysNet_GetHostByAddr(const void* addr, socklen_t len, int type, struct hostent** hostinfo);
void SysNet_FreeHostInfo(struct hostent* hostinfo);
int SysNet_SocketPair(SA_FAMILY_TYPE domain, SOCKET_TYPE type, int protocol, int sv[2]);
int SysIO_TCGetAttr(int fd, struct termios *termios_p);
int SysIO_TCSetAttr(int fd, termios_actions action, const struct termios *termios_p);
int SysIO_IsTTY(int fd);
int SysIO_CreatePipe(int fd[2]);
int SysIO_CreateNamedPipe(const char* path, int fd[2]);

#if defined __cplusplus
}
#endif
