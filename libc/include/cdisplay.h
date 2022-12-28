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

typedef struct {
  int _x;
  int _y;
  uint32_t _width;
  uint32_t _height;
} ViewportInfo;

extern void	SysDisplay_MoveCursor(int n) ;
extern void	SysDisplay_ClearLine(int pos) ;
extern void	SysDisplay_SetCursor(int pos, bool bUpdateCursorOnScreen) ;
extern int	SysDisplay_GetCursor() ;
extern void SysDisplay_GetConsoleSize(uint32_t*, uint32_t*);
extern void SysDisplay_InitGuiFrame(FrameBufferInfo*);
extern void SysDisplay_FrameTouch();
extern void SysDisplay_FrameHasAlpha(bool);
extern void SysDisplay_SetGuiBase(bool);
extern void SysDisplay_InitTermConsole();
extern void SysDisplay_InitGuiEventStream(int fdList[]);
extern void SysDisplay_SetViewport(const ViewportInfo*);
extern void SysDisplay_GetViewport(ViewportInfo*);

#define movcursor(n)		SysDisplay_MoveCursor(n)
#define clrline(pos)		SysDisplay_ClearLine(pos)
#define set_cursor(pos)		SysDisplay_SetCursor(pos, true)
#define set_cursor_val(pos)	SysDisplay_SetCursor(pos, false)
#define get_cursor()		SysDisplay_GetCursor()
#define console_size(mr, mc) SysDisplay_GetConsoleSize(mr, mc)
#define clrscr()			SysDisplay_ClearScreen()

void showprogress(int startCur, const char* msg, unsigned progNum) ;
void init_gui_frame(FrameBufferInfo* frameBufferInfo);
void gui_frame_touch();
void gui_frame_has_alpha(bool hasAlpha);
void set_gui_base(bool isGuiBase);
void init_term_console();
void init_gui_event_stream(int fdList[]);
void set_viewport(const ViewportInfo*);
void get_viewport(ViewportInfo*);

#if defined __cplusplus
}
#endif
