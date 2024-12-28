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

time_t time(time_t * t)
{
	time_t result;
	struct timeval tv;

	if (gettimeofday(&tv))	{
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

int gettimeofday(struct timeval* pTV)
{
  return SysUtil_GetTimeOfDay(pTV) ;
}

//in milliseconds
uint32_t btime() {
  return SysUtil_GetTimeSinceBoot();
}
