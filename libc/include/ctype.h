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
#ifndef _TYPES_H_
#define _TYPES_H_

#if defined __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define __isdigit_char(C)    (((unsigned char)((C) - '0')) <= 9)
#define __isdigit_int(C)     (((unsigned int)((C) - '0')) <= 9)

/* ASCII ords of \t, \f, \n, \r, and \v are 9, 12, 10, 13, 11 respectively. */
#define __C_isspace(c) \
	((sizeof(c) == sizeof(char)) \
	 ? ((((c) == ' ') || (((unsigned char)((c) - 9)) <= (13 - 9)))) \
	 : ((((c) == ' ') || (((unsigned int)((c) - 9)) <= (13 - 9)))))
#define __C_isblank(c) (((c) == ' ') || ((c) == '\t'))
#define __C_isdigit(c) \
	((sizeof(c) == sizeof(char)) \
	 ? (((unsigned char)((c) - '0')) < 10) \
	 : (((unsigned int)((c) - '0')) < 10))
#define __C_isxdigit(c) \
	(__C_isdigit(c) \
	 || ((sizeof(c) == sizeof(char)) \
		 ? (((unsigned char)((((c)) | 0x20) - 'a')) < 6) \
		 : (((unsigned int)((((c)) | 0x20) - 'a')) < 6)))
#define __C_iscntrl(c) \
	((sizeof(c) == sizeof(char)) \
	 ? ((((unsigned char)(c)) < 0x20) || ((c) == 0x7f)) \
	 : ((((unsigned int)(c)) < 0x20) || ((c) == 0x7f)))
#define __C_isalpha(c) \
	((sizeof(c) == sizeof(char)) \
	 ? (((unsigned char)(((c) | 0x20) - 'a')) < 26) \
	 : (((unsigned int)(((c) | 0x20) - 'a')) < 26))
#define __C_isalnum(c) (__C_isalpha(c) || __C_isdigit(c))
#define __C_isprint(c) \
	((sizeof(c) == sizeof(char)) \
	 ? (((unsigned char)((c) - 0x20)) <= (0x7e - 0x20)) \
	 : (((unsigned int)((c) - 0x20)) <= (0x7e - 0x20)))

#define __C_islower(c) \
    ((sizeof(c) == sizeof(char)) \
     ? (((unsigned char)((c) - 'a')) < 26) \
     : (((unsigned int)((c) - 'a')) < 26))
#define __C_isupper(c) \
    ((sizeof(c) == sizeof(char)) \
     ? (((unsigned char)((c) - 'A')) < 26) \
     : (((unsigned int)((c) - 'A')) < 26))
#define __C_isgraph(c) \
    ((sizeof(c) == sizeof(char)) \
     ? (((unsigned int)((c) - 0x21)) <= (0x7e - 0x21)) \
     : (((unsigned int)((c) - 0x21)) <= (0x7e - 0x21)))

#define __toascii(c) ((c) & 0x7f)
#define __C_tolower(c) (__C_isupper(c) ? ((c) | 0x20) : (c))
#define __C_toupper(c) (__C_islower(c) ? ((c) ^ 0x20) : (c))

int iswhitespace(int ch) ;
int isalpha(int ch) ;
int isalnum(int ch) ;
int isprint(int ch) ;
int isblank(int ch) ;
int isgraph(int ch) ;
int isdigit(int ch) ;
int isspace(int ch) ;
int iscntrl(int ch) ;
int isxdigit(int ch) ;
int toascii(int c) ;
int tolower(int c) ;
int islower(int c) ;
int toupper(int c) ;

//void memset(char* dest, int ch, int len) ;
//void* memcpy(char* dest, const char* src, int len) ;
//
#if defined __cplusplus
}
#endif

#endif
