/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2015 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
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

#include <stdlib.h>

char *strncat (char *s1, const char *s2, size_t n)
{
  char c;
  char *s = s1;

  /* Find the end of S1.  */
  do
    c = *s1++;
  while (c != '\0');

  /* Make S1 point before next character, so we can increment
     it while memory is read (wins on pipelined cpus).  */
  s1 -= 2;

  if (n >= 4)
  {
    size_t n4 = n >> 2;
    do
    {
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        return s;
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        return s;
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        return s;
      c = *s2++;
      *++s1 = c;
      if (c == '\0')
        return s;
    } while (--n4 > 0);
    n &= 3;
  }

  while (n > 0)
  {
    c = *s2++;
    *++s1 = c;
    if (c == '\0')
      return s;
    n--;
  }

  if (c != '\0')
    *++s1 = '\0';

  return s;
}