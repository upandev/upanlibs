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
#include <ctype.h>
#include <string.h>

int iswhitespace(int ch)
{
	return __C_isspace(ch) ;
}

int isdigit(int ch)
{
	return __C_isdigit(ch) ;
}

int isspace(int ch)
{
	return __C_isspace(ch) ;
}

int isxdigit(int ch)
{
	return __C_isxdigit(ch) ;
}

int isalpha(int ch)
{
	return __C_isalpha(ch) ;
}

int isprint(int ch)
{
	return __C_isprint(ch) ;
}

int isalnum(int ch)
{
	return __C_isalnum(ch) ;
}

int isblank(int ch)
{
	return __C_isblank(ch) ;
}

int isgraph(int ch)
{
	return __C_isgraph(ch) ;
}

int iscntrl(int ch)
{
	return __C_iscntrl(ch) ;
}

int toascii(int c)
{
	return __toascii(c) ; 
}

int tolower(int c)
{
    return __C_tolower(c);
}

int toupper(int c)
{
    return __C_toupper(c);
}
