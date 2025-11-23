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

#define SECONDS_IN_NON_LEAP_YEAR 31536000L
#define SECONDS_IN_LEAP_YEAR 31622400L
#define SECONDS_IN_DAY 86400L

static const int DAYS_IN_A_MONTH[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

time_t time(time_t *t) {
	time_t result;
	struct timeval tv;

	if (gettimeofday(&tv, NULL))	{
		result = (time_t)-1;
	} else {
		result = (time_t) tv.tv_sec;
	}

	if (t != NULL) {
		*t = result;
	}
	return result;
}

clock_t clock() {
	//stub
	return 0 ;
}

int gettimeofday(struct timeval* pTV, struct timezone* pTZ) {
  return SysUtil_GetTimeOfDay(pTV) ;
}

struct tm* gmtime_r(const time_t *timep, struct tm *result) {
  if (result == NULL || timep == NULL) {
    return NULL;
  }

  time_t t = *timep;
  int year = 1970;
  int totalDays = 0;
  const bool negativeTime = t < 0;

  if (negativeTime) {
    t = -t;
    year = 1969;
  }

  while (true) {
    bool isLeapYear = year % 4 == 0 ? true : false;
    if ((isLeapYear && t < SECONDS_IN_LEAP_YEAR) || (!isLeapYear && t < SECONDS_IN_NON_LEAP_YEAR)) {
      break;
    }
    t -= isLeapYear ? SECONDS_IN_LEAP_YEAR : SECONDS_IN_NON_LEAP_YEAR;
    if (negativeTime) {
      --year;
    } else {
      ++year;
    }
    totalDays += isLeapYear ? 366 : 365;
  }

  const bool isLeapYear = year % 4 == 0 ? true : false;
  if (negativeTime) {
    t = (isLeapYear ? SECONDS_IN_LEAP_YEAR : SECONDS_IN_NON_LEAP_YEAR) - t;
  }

  int month = 0;
  int dayOfTheYear = 0;
  for (; month < 12; ++month) {
    int days = DAYS_IN_A_MONTH[month];
    if (isLeapYear && month == 1) {
      days++;
    }

    const long secondsInMonth = days * SECONDS_IN_DAY;
    if (t < secondsInMonth) {
      break;
    }
    t -= secondsInMonth;
    dayOfTheYear += days;
  }

  int dayOfTheMonth = 1;
  while (t >= SECONDS_IN_DAY) {
    t -= SECONDS_IN_DAY;
    dayOfTheMonth++;
    dayOfTheYear++;
  }

  totalDays += (negativeTime ? (isLeapYear ? 366 : 365) - dayOfTheYear : dayOfTheYear);

  int hour = t / 3600;
  int minute = (t % 3600) / 60;
  int second = t % 60;
  result->tm_sec = second;
  result->tm_min = minute;
  result->tm_hour = hour;
  result->tm_mday = dayOfTheMonth;
  result->tm_mon = month;
  result->tm_year = year - 1900;
  if (negativeTime) {
    int w = (4 - totalDays) % 7;
    result->tm_wday = (w < 0 ? w + 7 : w);
  } else {
    result->tm_wday = (totalDays + 4) % 7;
  }
  result->tm_yday = dayOfTheYear;
  return result;
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

