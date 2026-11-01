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

#include <stdint.h>
#include <mosstd.h>

uint64_t rdtsc() {
  uint32_t lo, hi;
  __asm__ __volatile__("lfence\n\t"
                       "rdtsc" : "=a"(lo), "=d"(hi) : : "memory");
  return ((uint64_t)hi << 32) | lo;
}

time_t tscDurationStartEnd(uint64_t start, uint64_t end) {
  if (end < start) {
    return 0;
  }
  return (time_t)((end - start) * 1000000 / THREAD_LOCAL_META_DATA->_tscFreq);
}

time_t tscDurationStart(uint64_t start) {
  return tscDurationStartEnd(start, rdtsc());
}

time_t tscCurrentTime() {
  return THREAD_LOCAL_META_DATA->_bootTime + tscDurationStart(THREAD_LOCAL_META_DATA->_bootTsc);
}

