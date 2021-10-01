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
# include <syscalldefs.h>
# include <dtime.h>

void SysUtil_GetDateTime(RTCTime* pRTCTime)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(pRTCTime)) ;
	DO_SYS_CALL(SYS_CALL_UTIL_DTIME) ;
	__asm__ __volatile__("pop %eax") ;
}

void SysUtil_Reboot()
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	DO_SYS_CALL(SYS_CALL_UTIL_REBOOT) ;
	__asm__ __volatile__("pop %eax") ;
}

int SysUtil_GetTimeOfDay(struct timeval* pTV)
{
	__volatile__ int iRet ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(pTV)) ;
	DO_SYS_CALL(SYS_CALL_UTIL_TOD) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRet) : ) ;
	__asm__ __volatile__("pop %eax") ;

	return iRet ;
}

uint32_t SysUtil_GetTimeSinceBoot()
{
  __volatile__ int iRet ;

  __asm__ __volatile__("push %eax") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;

  DO_SYS_CALL(SYS_CALL_UTIL_BTIME) ;

  __asm__ __volatile__("movl %%eax, %0" : "=m"(iRet) : ) ;
  __asm__ __volatile__("pop %eax") ;

  return iRet ;
}