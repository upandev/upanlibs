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

#include <file_stream.h>
#include <fs.h>

namespace upan {
  file_stream::file_stream() : _fd(-1) {
  }

  file_stream::file_stream(const upan::string& fileName, int flags, mode_t mode) : file_stream() {
    open(fileName, flags, mode);
  }

  file_stream::file_stream(int fd) : _fd(fd) {
  }

  file_stream::~file_stream() {
    close();
  }

  void file_stream::close() {
    if(is_good()) {
      ::close(_fd);
    }
    _fd = -1;
    _line_buffer.close();
  }

  bool file_stream::open(const upan::string& fileName, int flags, mode_t mode) {
    close();
    return (_fd = ::open(fileName.c_str(), flags, mode)) >= 0;
  }

  int file_stream::read(uint8_t *buf, int n) const {
    return ::read(_fd, buf, n);
  }

  bool file_stream::read_line(upan::string& line) {
    _line_buffer.init();

    char buf[line_buffer::CHUNK_SIZE];

    upan::option<upan::string> br = _line_buffer.read_line();
    if (!br.isEmpty()) {
      line = br.value();
      return true;
    }

    while(true) {
      if (_line_buffer.capacity_exceeded()) {
        return false;
      }

      int n = read((uint8_t*)buf, line_buffer::CHUNK_SIZE);

      if (n < 0) {
        return false;
      }

      if (n == 0) {
        return _line_buffer.read_remaining().ifPresent([&line] (upan::string& v) { line = v; });
      }

      _line_buffer.add(buf, n);

      upan::option<upan::string> r = _line_buffer.read_line();
      if (!r.isEmpty()) {
        line = r.value();
        return true;
      }
    }
  }

  int file_stream::write(const char* buf, int len) {
    return ::write(_fd, buf, len);
  }

  bool file_stream::seek_set(int offset) {
    return lseek(_fd, offset, SEEK_SET) >= 0;
  }

  bool file_stream::seek_cur(int offset) {
    return lseek(_fd, offset, SEEK_CUR) >= 0;
  }

  bool file_stream::seek_end(int offset) {
    return lseek(_fd, offset, SEEK_END) >= 0;
  }

  file_stream::line_buffer::line_buffer() : _buf(nullptr), _size(0), _len(0) {
  }

  file_stream::line_buffer::~line_buffer() {
    if (_size) {
      delete []_buf;
      _buf = nullptr;
    }
  }

  void file_stream::line_buffer::init() {
    if (_size == 0) {
      _buf = new char[CHUNK_SIZE];
      _size = CHUNK_SIZE;
      _len = 0;
    }
  }

  void file_stream::line_buffer::close() {
    _len = 0;
  }

  bool file_stream::line_buffer::capacity_exceeded() const {
    return _size > MAX_LINE_SIZE;
  }

  void file_stream::line_buffer::add(const char* buf, int n) {
    const int new_size = _len + n;
    if (new_size <= _size) {
      memcpy(_buf + _len, buf, n);
    } else {
      char *new_buf = new char[new_size];
      memcpy(new_buf, _buf, _len);
      memcpy(new_buf + _len, buf, n);
      delete[] _buf;
      _buf = new_buf;
      _size = new_size;
    }
    _len = new_size;
  }

  upan::option<upan::string> file_stream::line_buffer::read_remaining() {
    if (_len > 0) {
      int len = _len;
      _len = 0;
      return upan::option<upan::string>(upan::string(_buf, len));
    }
    return upan::option<upan::string>::empty();
  }

  upan::option<upan::string> file_stream::line_buffer::read_line() {
    for (int n = 0; n < _len; ++n) {
      if (_buf[n] == '\n') {
        upan::string line(_buf, n);
        ++n;
        _len -= n;
        memcpy(_buf, _buf + n, _len);
        return upan::option<upan::string>(line);
      }
    }
    return upan::option<upan::string>::empty();
  }
}