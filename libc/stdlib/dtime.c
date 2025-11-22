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
# include <dtime.h>
# include <syscalldefs.h>
# include <stdio.h>

time_t time(time_t * t)
{
	time_t result;
	struct timeval tv;

	if (gettimeofday(&tv, NULL))	{
		result = (time_t) - 1;
	} else {
		result = (time_t) tv.tv_sec;
	}

	if (t != NULL) 
	{
		*t = result;
	}
	return result;
}

clock_t clock()
{
	//stub
	return 0 ;
}

int gettimeofday(struct timeval* pTV, struct timezone* pTZ)
{
  return SysUtil_GetTimeOfDay(pTV) ;
}

//in milliseconds
uint32_t btime() {
  return SysUtil_GetTimeSinceBoot();
}

void dtime(RTCDateTime* rtcDateTime) {
  SysUtil_GetDateTime(rtcDateTime);
}

__thread char _dtime_str_buf[30];

char* dtime_str() {
  RTCDateTime rtcDateTime;
  dtime(&rtcDateTime);

  sprintf(_dtime_str_buf, "%02d/%02d/%d %02d:%02d:%02d", rtcDateTime._dayOfMonth, rtcDateTime._month,
          rtcDateTime._century * 100 + rtcDateTime._year, rtcDateTime._hour, rtcDateTime._minute,
          rtcDateTime._second);

  return _dtime_str_buf;
}

int sleep(uint32_t s) {
  return SysProcess_Sleep(s * 1000);
}

int sleepms(uint32_t ms) {
  return SysProcess_Sleep(ms);
}

int nanosleep(const struct timespec* req, struct timespec* rem) {
  if (!req) {
    return 0;
  }
  time_t ms = req->tv_sec * 1000 + req->tv_nsec / 1000000;
  int r = sleepms(ms);
  if (r && rem) {
    rem->tv_sec = r / 1000;
    rem->tv_nsec = (r % 1000) * 1000000;
  }
  return r >= 0 ? 0 : -1;
}

