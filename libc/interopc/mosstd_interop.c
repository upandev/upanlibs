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
# include <mosstd.h>
# include <syscalldefs.h>

static process_init_fini_t* _process_init_fini_list = NULL;

extern void load_environ(const char** environ);
extern void __cxa_finalize(void*);
extern void _stdio_term();

//this is used inside crt start-up code - called at first before transferring control to main()
UNUSED void _process_init_relocate(int argc, char** argv) {
  _process_init_fini_list = SysProcess_InitRelocate();
  process_init_fini_t* i = _process_init_fini_list;

  //the last entry is main executable
  while (!i->_end) {
    if (i->_init) {
      i->_init();
    }
    i++;
  }

  uint32_t argvSize = sizeof(uintptr_t) * (argc + 1);
  int e;
  for(e = 0; e < argc; ++e) {
    argvSize += strlen(argv[e]) + 1;
  }
  load_environ((const char**)((uintptr_t)argv + argvSize));
}

void _user_process_exit() {
  _stdio_term();
  __cxa_finalize(NULL);

  //the last entry is for the main executable
  process_init_fini_t* i = _process_init_fini_list;
  while (!i->_end) {
    if (i->_fini) {
      i->_fini();
    }
    i++;
  }
}