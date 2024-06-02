/*
 *  Upanix - An x86 based Operating System
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
#include <cdisplay.h>
#include <stdio.h>

void showprogress(int startCur, const char* msg, unsigned progNum)
{
  int c = get_cursor() ;

  while(c > startCur)
  {
    movcursor(-1) ;
    c-- ;
  }

  clrline(Display_CURSOR_CUR) ;

  printf("%s%d", msg, progNum) ;
}

void init_gui_frame(FrameBufferInfo* frameBufferInfo) {
  SysDisplay_InitGuiFrame(frameBufferInfo);
}

void gui_frame_touch() {
  SysDisplay_FrameTouch();
}

void gui_frame_has_alpha(bool hasAlpha) {
  SysDisplay_FrameHasAlpha(hasAlpha);
}

void set_gui_base(bool isGuiBase) {
  SysDisplay_SetGuiBase(isGuiBase);
}

void set_mouse_cursor_type(MouseCursorType type) {
  SysDisplay_SetMouseCursorType(type);
}

void init_term_console() {
  SysDisplay_InitTermConsole();
}

void init_gui_event_stream(int fdList[]) {
  SysDisplay_InitGuiEventStream(fdList);
}

void set_viewport(const ViewportInfo* viewportInfo) {
  SysDisplay_SetViewport(viewportInfo);
}

void get_viewport(ViewportInfo* viewportInfo) {
  SysDisplay_GetViewport(viewportInfo);
}