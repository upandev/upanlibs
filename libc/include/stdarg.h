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
#ifndef _STD_ARG_H_
#define _STD_ARG_H_

typedef void* va_list ;

#if defined __cplusplus
extern "C" {
#endif

//to avoid pointer arithmetic on void* warning, add sizeof(unsigned) to unsigned value of the address and then cast to va_list (aka void*)
#define va_start(var_list, last_known_arg) \
var_list = (va_list)((unsigned)&last_known_arg + sizeof(unsigned)) ;

#define va_end(var_list) ;

#define va_arg(var_list, cur_type) \
(var_list = var_list + sizeof(cur_type), *(cur_type*)(var_list - sizeof(cur_type)))

#define va_copy(var_list_dest, var_list_src) \
var_list_dest = var_list_src ;

#if defined __cplusplus
}
#endif

#endif
