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
#include <file_stream.h>

namespace upan {
  class logger {
  public:
    enum level_t {
      LOG_DEBUG = 1,
      LOG_INFO = 2,
      LOG_WARN = 4,
      LOG_ERROR = 8,
    };
  private:
    static logger* _instance;

    explicit logger(const upan::string& filePath);
    explicit logger(int fd);
    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

  public:
    static void create(const upan::string& filePath);
    static void create(int fd);
    static void close();
    static logger& instance();

    void log(level_t level, const char * __restrict fmsg, ...);
    void logarg(level_t level, const char * __restrict fmsg, va_list);

  private:
    void _log(level_t level, const upan::string& msg);
    file_stream _writer;
  };
}