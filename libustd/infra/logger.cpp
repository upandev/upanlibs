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
#include <syslog.h>

static constexpr int DEFAULT_LOG_MASK = LOG_UPTO(LOG_PRIORITY::LOG_EMERG);

namespace upan {
logger::logger(const upan::string& ident, int option, LOG_CATEGORY facility) :
        _mask(DEFAULT_LOG_MASK), _logRepeatCount(0),
        _ident(ident), _option(option), _facility(facility) {
  disable(LOG_PRIORITY::LOG_DEBUG);
}

logger::logger(const upan::string& filePath, const upan::string& ident, int option, LOG_CATEGORY facility) :
        logger(ident, option, facility) {
  _writer.open(filePath, O_RDWR | O_APPEND);
}

logger::logger(const upan::string& filePath) : logger(filePath, "", LOG_PID | LOG_CONS, LOG_USER) {
}

logger::logger() : logger("", LOG_PID | LOG_CONS, LOG_USER) {
}

void logger::openFile(const upan::string& filePath) {
  _writer.open(filePath, O_RDWR | O_APPEND);
}

void logger::closeFile() {
  _writer.close();
}

void logger::enable(LOG_PRIORITY priority) {
  _mask.bit_or(1 << priority);
}

void logger::disable(LOG_PRIORITY priority) {
  _mask.bit_and(~(1 << priority));
}

void logger::setMaskUpto(LOG_PRIORITY priority) {
  _mask.set(LOG_UPTO(priority));
}

void logger::setMask(uint32_t mask) {
  _mask.set(mask);
}

void logger::enable(const upan::string& priority) {
  enable((LOG_PRIORITY)str_to_log_priority(priority.c_str()));
}

void logger::disable(const upan::string& priority) {
  disable((LOG_PRIORITY)str_to_log_priority(priority.c_str()));
}

void logger::log(LOG_PRIORITY priority, const char * __restrict fmsg, ...) {
  upan::mutex_guard g(_logMutex);
  if (!(_mask.get() & (1 << priority))) { return; }

  va_list arg;
  va_start(arg, fmsg);
  _logarg(priority, fmsg, arg);
  va_end(arg);
}

void logger::log(LOG_PRIORITY priority, const char * __restrict fmsg, va_list arg) {
  upan::mutex_guard g(_logMutex);
  if (!(_mask.get() & priority)) { return; }

  _logarg(priority, fmsg, arg);
}

void logger::log(const upan::string& msg) {
  upan::mutex_guard g(_logMutex);
  _log(msg);
}

void logger::_logarg(LOG_PRIORITY priority, const char * __restrict fmsg, va_list arg) {
  construct_log_msg(_messageBuffer, MAX_LOG_MESSAGE_SIZE, priority, _ident.c_str(), _option, _facility, fmsg, arg);
  _log(_messageBuffer);
}

void logger::_log(const upan::string& msg) {
  upan::string logline("\n");
  logline += dtime_str();

  static upan::string REPEATED_PREFIX("[REPEATED ");
  static upan::string REPEATED_SUFFIX(" TIMES]");

  if (_prevLogMsg == msg) {
    _logRepeatCount++;
    if (_logRepeatCount % 100 == 0) {
      const upan::string countStr(upan::string::to_string(_logRepeatCount));
      logline += REPEATED_PREFIX + countStr + REPEATED_SUFFIX;
      _writer.write(logline.c_str(), logline.length());
      return;
    }
  } else {
    if (_logRepeatCount > 0) {
      const upan::string countStr(upan::string::to_string(_logRepeatCount));
      const upan::string repeatLogLine = logline + REPEATED_PREFIX + countStr + REPEATED_SUFFIX;
      _logRepeatCount = 0;
      _writer.write(repeatLogLine.c_str(), repeatLogLine.length());
    }
    _prevLogMsg = msg;
  }

  logline += msg;
  if (_writer.is_good()) {
    _writer.write(logline.c_str(), logline.length());
  } else if (_option & LOG_CONS) {
    printf("%s", logline.c_str());
  }
}

}