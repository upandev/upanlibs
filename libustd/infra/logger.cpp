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

#include <logger.h>
#include <stdarg.h>

namespace upan {
logger* logger::_instance = nullptr;

void logger::create(const upan::string& filePath) {
  if (_instance) {
    throw upan::exception(XLOC, "logger is already created");
  }
  _instance = new logger(filePath);
}

void logger::close() {
  delete _instance;
  _instance = nullptr;
}

logger& logger::instance() {
  if (!_instance) {
    throw upan::exception(XLOC, "logger is not created yet");
  }
  return *_instance;
}

logger::logger(const upan::string& filePath) : _writer(filePath, O_RDWR | O_APPEND) {
}

void logger::log(const char * __restrict fmsg, ...) {
  va_list arg;
  va_start(arg, fmsg);
  logarg(fmsg, arg);
  va_end(arg);
}

void logger::logarg(const char * __restrict fmsg, va_list arg) {
  const int BSIZE = 1024;
  char buf[BSIZE];
  vsnprintf(buf, BSIZE, fmsg, arg);

  if (logger::is_good()) {
    logger::instance()._log(buf);
  } else {
    printf(buf);
  }
}

void logger::_log(const upan::string& msg) {
  _writer.write(msg.c_str(), msg.length());
}

}