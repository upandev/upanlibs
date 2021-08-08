/*
 *  Mother Operating System - An x86 based Operating System
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
# include <syscalldefs.h>
# include <ctype.h>
#include <cdisplay.h>

void SysDisplay_Message(const char* szMessage, unsigned uiAttr)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(uiAttr)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(szMessage)) ;
	DO_SYS_CALL(SYS_CALL_DISPLAY_MESSAGE) ;
	__asm__ __volatile__("pop %eax") ;
}

void SysDisplay_ClearScreen()
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	DO_SYS_CALL(SYS_CALL_DISPLAY_CLR_SCR) ;
	__asm__ __volatile__("pop %eax") ;
}

void SysDisplay_MoveCursor(int n)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(n)) ;
	DO_SYS_CALL(SYS_CALL_DISPLAY_MOV_CURSOR) ;
	__asm__ __volatile__("pop %eax") ;
}

void SysDisplay_ClearLine(int pos)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(pos)) ;
	DO_SYS_CALL(SYS_CALL_DISPLAY_CLR_LINE) ;
	__asm__ __volatile__("pop %eax") ;
}

void SysDisplay_SetCursor(__volatile__ int iCurPos, __volatile__ bool bUpdateCursorOnScreen)
{
	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "m"(bUpdateCursorOnScreen)) ;
	__asm__ __volatile__("pushl %0" : : "m"(iCurPos)) ;
	DO_SYS_CALL(SYS_CALL_DISPLAY_SET_CURSOR) ;
	__asm__ __volatile__("pop %eax") ;
}

int SysDisplay_GetCursor()
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	DO_SYS_CALL(SYS_CALL_DISPLAY_GET_CURSOR) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
	return iRetStatus ;
}

void SysDisplay_RawCharacter(__volatile__ const char ch, __volatile__ unsigned uiAttr, __volatile__ bool bUpdateCursorOnScreen)
{
	__volatile__ unsigned v = ch ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"((int)bUpdateCursorOnScreen)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(uiAttr)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(v)) ;

	DO_SYS_CALL(SYS_CALL_DISPLAY_RAW_CHAR) ;
	__asm__ __volatile__("pop %eax") ;
}

void SysDisplay_RawCharacterArea(const MChar* src, uint32_t rows, uint32_t cols, int curPos) {
  __asm__ __volatile__("push %eax") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;

  __asm__ __volatile__("pushl %0" : : "rm"(curPos)) ;
  __asm__ __volatile__("pushl %0" : : "rm"(cols)) ;
  __asm__ __volatile__("pushl %0" : : "rm"(rows)) ;
  __asm__ __volatile__("pushl %0" : : "rm"(src)) ;

  DO_SYS_CALL(SYS_CALL_DISPLAY_RAW_CHAR_AREA) ;
  __asm__ __volatile__("pop %eax") ;
}

void SysDisplay_GetConsoleSize(unsigned* retMaxRows, unsigned* retMaxCols)
{
	__volatile__ int iRetStatus ;

	__asm__ __volatile__("push %eax") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;
	__asm__ __volatile__("pushl $0x20") ;

	__asm__ __volatile__("pushl %0" : : "rm"(retMaxCols)) ;
	__asm__ __volatile__("pushl %0" : : "rm"(retMaxRows)) ;
	DO_SYS_CALL(SYS_CALL_DISPLAY_CONSOLE_SIZE) ;

	__asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
	__asm__ __volatile__("pop %eax") ;
}

void SysDisplay_InitGui(FrameBufferInfo* frameBufferInfo) {
  __volatile__ int iRetStatus ;

  __asm__ __volatile__("push %eax") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;

  __asm__ __volatile__("pushl %0" : : "rm"(frameBufferInfo)) ;
  DO_SYS_CALL(SYS_CALL_DISPLAY_INIT_GUI) ;

  __asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
  __asm__ __volatile__("pop %eax") ;
}

void SysDisplay_FrameTouch() {
  __volatile__ int iRetStatus ;

  __asm__ __volatile__("push %eax") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;
  __asm__ __volatile__("pushl $0x20") ;

  DO_SYS_CALL(SYS_CALL_DISPLAY_FRAME_TOUCH) ;

  __asm__ __volatile__("movl %%eax, %0" : "=m"(iRetStatus) : ) ;
  __asm__ __volatile__("pop %eax") ;
}