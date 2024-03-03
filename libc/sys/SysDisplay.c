/*
 *  Mother Operating System - An x86 based Operating System
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
# include <syscalldefs.h>
# include <ctype.h>
#include <cdisplay.h>

void SysDisplay_Message(const char* szMessage, unsigned uiAttr) {
  _upanix_syscall(SYS_CALL_DISPLAY_MESSAGE, (uint64_t)szMessage, (uint64_t)uiAttr, 3, 4, 5);
}

void SysDisplay_ClearScreen() {
  _upanix_syscall(SYS_CALL_DISPLAY_CLR_SCR, 1, 2, 3, 4, 5);
}

void SysDisplay_MoveCursor(int n) {
  _upanix_syscall(SYS_CALL_DISPLAY_MOV_CURSOR, (uint64_t)n, 2, 3, 4, 5);
}

void SysDisplay_ClearLine(int pos) {
  _upanix_syscall(SYS_CALL_DISPLAY_CLR_LINE, (uint64_t)pos, 2, 3, 4, 5);
}

void SysDisplay_SetCursor(int iCurPos, bool bUpdateCursorOnScreen) {
  _upanix_syscall(SYS_CALL_DISPLAY_SET_CURSOR, (uint64_t)iCurPos, (uint64_t)bUpdateCursorOnScreen, 3, 4, 5);
}

int SysDisplay_GetCursor() {
  return _upanix_syscall(SYS_CALL_DISPLAY_GET_CURSOR, 1, 2, 3, 4, 5);
}

void SysDisplay_RawCharacter(const char ch, unsigned uiAttr, bool bUpdateCursorOnScreen) {
  _upanix_syscall(SYS_CALL_DISPLAY_RAW_CHAR, (uint64_t)ch, (uint64_t)uiAttr, (uint64_t)bUpdateCursorOnScreen, 4, 5);
}

void SysDisplay_RawCharacterArea(const MChar* src, uint32_t rows, uint32_t cols, int curPos) {
  _upanix_syscall(SYS_CALL_DISPLAY_RAW_CHAR_AREA, (uint64_t)src, (uint64_t)rows, (uint64_t)cols, (uint64_t)curPos, 5);
}

void SysDisplay_GetConsoleSize(unsigned* retMaxRows, unsigned* retMaxCols) {
  _upanix_syscall(SYS_CALL_DISPLAY_CONSOLE_SIZE, (uint64_t)retMaxRows, (uint64_t)retMaxCols, 3, 4, 5);
}

void SysDisplay_InitGuiFrame(FrameBufferInfo* frameBufferInfo) {
  _upanix_syscall(SYS_CALL_DISPLAY_INIT_GUI_FRAME, (uint64_t)frameBufferInfo, 2, 3, 4, 5);
}

void SysDisplay_FrameTouch() {
  _upanix_syscall(SYS_CALL_DISPLAY_GUI_FRAME_TOUCH, 1, 2, 3, 4, 5);
}

void SysDisplay_FrameHasAlpha(bool hasAlpha) {
  _upanix_syscall(SYS_CALL_DISPLAY_GUI_FRAME_HAS_ALPHA, (uint64_t)hasAlpha, 2, 3, 4, 5);
}

void SysDisplay_SetGuiBase(bool isGuiBase) {
  _upanix_syscall(SYS_CALL_DISPLAY_SET_GUI_BASE, (uint64_t)isGuiBase, 2, 3, 4, 5);
}

void SysDisplay_InitTermConsole() {
  _upanix_syscall(SYS_CALL_DISPLAY_INIT_TERM_CONSOLE, 1, 2, 3, 4, 5);
}

void SysDisplay_InitGuiEventStream(int fdList[]) {
  _upanix_syscall(SYS_CALL_DISPLAY_INIT_GUI_EVENT_STREAM, (uint64_t)fdList, 2, 3, 4, 5);
}

void SysDisplay_SetViewport(const ViewportInfo* viewportInfo) {
  _upanix_syscall(SYS_CALL_DISPLAY_SET_VIEWPORT, (uint64_t)viewportInfo, 2, 3, 4, 5);
}

void SysDisplay_GetViewport(ViewportInfo* viewportInfo) {
  _upanix_syscall(SYS_CALL_DISPLAY_GET_VIEWPORT, (uint64_t)viewportInfo, 2, 3, 4, 5);
}