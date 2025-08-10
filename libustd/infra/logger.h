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

#include <ustring.h>
#include <atomicop.h>
#include <file_stream.h>
#include <mutex.h>
#include "syslog.h"

namespace upan {
  class logger {
  public:

    logger(const upan::string& ident, int option, LOG_CATEGORY facility);
    logger(const upan::string& filePath, const upan::string& ident, int option, LOG_CATEGORY facility);
    explicit logger(const upan::string& filePath);
    explicit logger();

    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

    void openFile(const upan::string& filePath);
    void closeFile();
    void log(LOG_PRIORITY priority, const char * __restrict fmsg, ...);
    void log(LOG_PRIORITY priority, const char * __restrict fmsg, va_list arg);
    void log(const upan::string& msg);

    void enable(LOG_PRIORITY priority);
    void disable(LOG_PRIORITY priority);
    void setMaskUpto(LOG_PRIORITY priority);
    void setMask(uint32_t logmask);
    void enable(const upan::string& priority);
    void disable(const upan::string& priority);

  private:
    void _logarg(LOG_PRIORITY priority, const char * __restrict fmsg, va_list);
    void _log(const upan::string& msg);

    file_stream _writer;
    char _messageBuffer[MAX_LOG_MESSAGE_SIZE];
    upan::atomic::integral<uint32_t> _mask;
    upan::string _prevLogMsg;
    int _logRepeatCount;
    upan::string _ident;
    int _option;
    LOG_CATEGORY _facility;
    upan::mutex _logMutex;
  };
}