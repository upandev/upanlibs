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
#include <uniq_ptr.h>
#include "fs.h"

namespace upan {
  class file_stream final {
  public:
    file_stream();
    file_stream(const upan::string& fileName, uint8_t mode);
    file_stream(int fd);
    ~file_stream();

    bool is_good() const { return _fd >= 0; }
    bool open(const upan::string& fileName, uint8_t mode);
    int read(uint8_t* buf, int n) const;
    bool read_line(upan::string& line);
    int write(const char* buf, int len);
    bool seek_set(int offset);
    bool seek_cur(int offset);
    bool seek_end(int offset);

  private:
    void close();

    class line_buffer {
    public:
      static const int CHUNK_SIZE = 512;
      static const int MAX_LINE_SIZE = 32 * 1024;

      line_buffer();
      ~line_buffer();

      void init();
      void close();
      bool capacity_exceeded() const;
      void add(const char* buf, int n);
      upan::option<upan::string> read_remaining();
      upan::option<upan::string> read_line();

    private:
      char* _buf;
      int _size;
      int _len;
    };

  private:
    upan::string _file_name;
    int _fd;
    line_buffer _line_buffer;
  };
}