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
#include <dirent.h>
#include <fs.h>
#include <syslog.h>
#include <stdlib.h>
#include <file_util.h>

#define LOG_SIZE_ROTATTION_THRESHOLD (10 * 1024)

static constexpr int DEFAULT_LOG_MASK = LOG_UPTO(LOG_PRIORITY::LOG_EMERG);

namespace upan {
namespace {

bool isRotationFile(const char* entryName, const upan::string& baseName, int& sequence) {
  const int baseLength = baseName.length();
  const int entryLength = strlen(entryName);
  if (entryLength <= baseLength ||
      strncmp(entryName, baseName.c_str(), baseLength) != 0 ||
      entryName[baseLength] != '.') {
    return false;
  }

  const char* suffix = entryName + baseLength + 1;
  if (*suffix == '\0') {
    return false;
  }

  for (const char* p = suffix; *p != '\0'; ++p) {
    if (*p < '0' || *p > '9') {
      return false;
    }
  }

  sequence = atoi(suffix);
  return true;
}

int maxRotationSequenceFor(const upan::string& filePath) {
  const upan::string dirPath(upan::file_path::dirname(filePath));
  const upan::string baseName(upan::file_path::basename(filePath));

  DIR* dir = opendir(dirPath.c_str());
  if (dir == nullptr) {
    return 0;
  }

  int maxSequence = 0;
  struct dirent* entry = nullptr;
  while ((entry = readdir(dir)) != nullptr) {
    int sequence = 0;
    if (isRotationFile(entry->d_name, baseName, sequence) && sequence > maxSequence) {
      maxSequence = sequence;
    }
  }

  closedir(dir);
  return maxSequence;
}

}

logger::logger(const upan::string& ident, int option, LOG_CATEGORY facility) :
        _mask(DEFAULT_LOG_MASK), _logRepeatCount(0),
        _ident(ident), _option(option), _facility(facility), _lastLogTime(0), _logSize(0),
        _maxRotationLogSequence(0) {
  disable(LOG_PRIORITY::LOG_DEBUG);
}

logger::logger(const upan::string& filePath, const upan::string& ident, int option, LOG_CATEGORY facility) :
        logger(ident, option, facility) {
  openFile(filePath);
}

logger::logger(const upan::string& filePath) : logger(filePath, "", LOG_PID | LOG_CONS, LOG_USER) {
}

logger::logger() : logger("", LOG_PID | LOG_CONS, LOG_USER) {
}

void logger::openFile(const upan::string& filePath) {
  _writer.open(filePath, O_RDWR | O_APPEND, ATTR_FILE_DEFAULT);
  _logFilePath = filePath;
  _maxRotationLogSequence = maxRotationSequenceFor(filePath);
  _logSize = _writer.size();
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
  if (!(_mask.get() & (1 << priority))) { return; }

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
  time_t now = time(nullptr);
  logline += dtime_str();

  static upan::string REPEATED_PREFIX(" [REPEATED ");
  static upan::string REPEATED_SUFFIX(" TIMES] ");

  if (_prevLogMsg == msg) {
    _logRepeatCount++;
    if (_logRepeatCount % 100 == 0 || (now - _lastLogTime) >= 60) {
      const upan::string countStr(upan::string::to_string(_logRepeatCount));
      logline += REPEATED_PREFIX + countStr + REPEATED_SUFFIX;
      _logRepeatCount = 0;
    } else {
      return;
    }
  } else {
    if (_logRepeatCount > 0) {
      const upan::string countStr(upan::string::to_string(_logRepeatCount));
      const upan::string repeatLogLine = logline + REPEATED_PREFIX + countStr + REPEATED_SUFFIX + _prevLogMsg;
      _logRepeatCount = 0;
      _write(repeatLogLine);
    }
    _prevLogMsg = msg;
  }

  logline += msg;
  _write(logline);
}

void logger::rotate() {
  if (_logSize > LOG_SIZE_ROTATTION_THRESHOLD) {
    closeFile();
    const upan::string rotatedFilePath =
        _logFilePath + "." + upan::string::to_string(++_maxRotationLogSequence);
    rename(_logFilePath.c_str(), rotatedFilePath.c_str());
    _writer.open(_logFilePath, O_RDWR | O_APPEND, ATTR_FILE_DEFAULT);
    _logSize = 0;
  }
}

void logger::_write(const upan::string& logline) {
  _lastLogTime = time(nullptr);
  if (_writer.is_good()) {
    _writer.write(logline.c_str(), logline.length());
    _logSize += logline.length();
    rotate();
  } else if (_option & LOG_CONS) {
    printf("%s", logline.c_str());
  }
}

}
