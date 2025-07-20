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

void logger::create(int fd) {
  if (_instance) {
    throw upan::exception(XLOC, "logger is already created");
  }
  _instance = new logger(fd);
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

static uint32_t constexpr DEFAULT_LOG_LEVEL = upan::logger::LOG_INFO | upan::logger::LOG_WARN | upan::logger::LOG_ERROR;

logger::logger(const upan::string& filePath) : _writer(filePath, O_RDWR | O_APPEND), _logLevel(DEFAULT_LOG_LEVEL) {
}

logger::logger(int fd) : _writer(fd), _logLevel(DEFAULT_LOG_LEVEL) {
}

void logger::enable(uint32_t levels) {
  _logLevel.bit_or(levels);
}

void logger::disable(uint32_t levels) {
  _logLevel.bit_and(~levels);
}

void logger::log(level_t level, const char * __restrict fmsg, ...) {
  if (!(_logLevel.get() & level)) { return; }

  va_list arg;
  va_start(arg, fmsg);
  logarg(level, fmsg, arg);
  va_end(arg);
}

void logger::logarg(level_t level, const char * __restrict fmsg, va_list arg) {
  if (!(_logLevel.get() & level)) { return; }

  const int BSIZE = 1024;
  char buf[BSIZE];
  vsnprintf(buf, BSIZE, fmsg, arg);
  _log(level, buf);
}

void logger::_log(level_t level, const upan::string& msg) {
  upan::string logline("\n");
  logline += dtime_str();
  switch (level) {
    case LOG_TRACE:
      logline += " [TRACE] ";
      break;

    case LOG_DEBUG:
      logline += " [DEBUG] ";
      break;

    case LOG_INFO:
      logline += " [INFO] ";
      break;

    case LOG_WARN:
      logline += " [WARN] ";
      break;

    case LOG_ERROR:
      logline += " [ERROR] ";
      break;

    default:
      logline += " [UNKNOWN] ";
      break;
  }
  logline += msg;
  _writer.write(logline.c_str(), logline.length());
}

}