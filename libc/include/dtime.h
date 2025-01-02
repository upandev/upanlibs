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
#ifndef _DTIME_H_
#define _DTIME_H_

#if defined __cplusplus
extern "C" {
#endif

#include <stdlib.h>

typedef int64_t time_t;
typedef int64_t suseconds_t;
typedef int clock_t;

typedef struct {
	uint8_t _second ;
	uint8_t _minute ;
	uint8_t _hour ;

	uint8_t _dayOfWeek ;
	uint8_t _dayOfMonth ;
	uint8_t _month ;
	uint8_t _century ;
	uint8_t _year ;
} RTCDateTime ;

struct tm
{
	int tm_sec;         /* seconds */
	int tm_min;         /* minutes */
	int tm_hour;        /* hours */

	int tm_wday;        /* day of the week */
	int tm_mday;        /* day of the month */
	int tm_mon;         /* month */
	int tm_year;        /* year */
//	int tm_yday;        /* day in the year */
//	int tm_isdst;       /* daylight saving time */
} ;

time_t time(time_t * t) ;
int localtime(struct tm* t, const time_t* time) ;

struct timeval
{
	time_t tv_sec; //seconds
  time_t tv_usec; //microseconds
//  unsigned uimSec ;
} PACKED ;

#define HRS_IN_DAY (86400)
extern void SysUtil_GetDateTime(RTCDateTime* rtcDateTime) ;
#define dtime(prtc) SysUtil_GetDateTime(prtc)

int gettimeofday(struct timeval*) ;
//time since boot
uint32_t btime();

// As per POSIX...
#define CLOCKS_PER_SEC 1000000
clock_t clock() ;


#if defined __cplusplus
}
#endif


#endif
