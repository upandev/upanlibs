/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
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
#pragma once
#include <ctype.h>
#include <stdint.h>

#define Display_CURSOR_CUR -1
#define Display_WHITE_ON_BLACK 0x07

#if defined __cplusplus
extern "C" {
#endif

#define NO_CHAR				234

typedef struct {
  uint8_t _ch;
  uint8_t	_attr;
} MChar;

typedef struct {
  uint32_t _pitch;
  uint32_t _width;
  uint32_t _height;
  uint32_t _bpp;
  uint32_t* _frameBuffer;
} FrameBufferInfo;

extern void	SysDisplay_MoveCursor(int n) ;
extern void	SysDisplay_ClearLine(int pos) ;
extern void	SysDisplay_SetCursor(int pos, bool bUpdateCursorOnScreen) ;
extern int	SysDisplay_GetCursor() ;
extern void SysDisplay_GetConsoleSize(uint32_t*, uint32_t*);
extern void SysDisplay_InitGui(FrameBufferInfo*);
extern void SysDisplay_FrameTouch();

#define movcursor(n)		SysDisplay_MoveCursor(n)
#define clrline(pos)		SysDisplay_ClearLine(pos)
#define set_cursor(pos)		SysDisplay_SetCursor(pos, true)
#define set_cursor_val(pos)	SysDisplay_SetCursor(pos, false)
#define get_cursor()		SysDisplay_GetCursor()
#define console_size(mr, mc) SysDisplay_GetConsoleSize(mr, mc)

void showprogress(int startCur, const char* msg, unsigned progNum) ;
void init_gui(FrameBufferInfo* frameBufferInfo);
void gui_frame_touch();

#if defined __cplusplus
}
#endif
