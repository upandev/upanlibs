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
#include <ConfigFileDB.h>
#include <file_stream.h>

namespace upan {
  ConfigFileDB::ConfigFileDB(const upan::string& filePath, upan::ConfigFileDB::OpType opType) : _filePath(filePath), _opType(opType), _batchWrite(false) {
    upan::file_stream rstream(filePath, (opType == OpType::RDONLY ? O_RDONLY : (O_RDWR | O_CREAT)));

    if (!rstream.is_good()) {
      upan::exception(XLOC, "failed to open/load config-file %s", filePath.c_str());
    }

    upan::string line;
    while (rstream.read_line(line)) {
      line = upan::string::trim(line);
      if (line[0] == '#') {
        _items.push_back(upan::make_pair(upan::string::EMPTY, line));
      } else {
        upan::string key, value;
        if (line.split('=', key, value)) {
          _items.push_back(upan::make_pair(upan::string::trim(key), upan::string::trim(value)));
        }
      }
    }
  }

  upan::option<upan::string> ConfigFileDB::get(const upan::string& key) {
    if (!key.empty()) {
      for (const auto& i: _items) {
        if (i.first == key) {
          return upan::option<upan::string>(i.second);
        }
      }
    }
    return upan::option<upan::string>::empty();
  }

  void ConfigFileDB::set(const upan::string& key, const upan::string& value, const upan::string& comment) {
    if (key.empty()) {
      throw upan::exception(XLOC, "key can't be empty");
    }

    upan::string commentLine;
    if (!comment.empty()) {
      commentLine = upan::string("#") + comment;
    }

    bool updated = false;
    Items::iterator prevIt = _items.end();
    for (auto it = _items.begin(); it != _items.end(); ++it) {
      auto& item = *it;
      if (item.first == key) {
        updated = true;
        item.second = value;
        if (!commentLine.empty()) {
          if (prevIt != _items.end()) {
            auto& prevItem = *prevIt;
            if (prevItem.first.empty()) {
              prevItem.second = commentLine;
            } else {
              _items.insert(it, upan::make_pair(upan::string::EMPTY, commentLine));
            }
          } else {
            _items.push_front(upan::make_pair(upan::string::EMPTY, commentLine));
          }
        }
        break;
      }
      prevIt = it;
    }

    if (!updated) {
      if (!commentLine.empty()) {
        _items.push_back(upan::make_pair(upan::string::EMPTY, commentLine));
      }
      _items.push_back(upan::make_pair(key, value));
    }

    writeToConfigFile();
  }

  void ConfigFileDB::remove(const upan::string& key) {
    if (key.empty()) {
      throw upan::exception(XLOC, "key can't be empty");
    }

    bool updated = false;
    Items::iterator prevIt = _items.end();
    for (auto it = _items.begin(); it != _items.end(); ++it) {
      auto& item = *it;
      if (item.first == key) {
        updated = true;
        _items.erase(it);
        if (prevIt != _items.end() && (*prevIt).first.empty()) {
          _items.erase(prevIt);
        }
        break;
      }
      prevIt = it;
    }

    if (updated) {
      writeToConfigFile();
    }
  }


  void ConfigFileDB::writeToConfigFile() {
    if (_opType == OpType::RDWR && _batchWrite == false) {
      upan::file_stream wstream(_filePath, O_RDWR | O_TRUNC);
      if (!wstream.is_good()) {
        throw upan::exception(XLOC, "failed to open config-file %s for writing", _filePath.c_str());
      }

      for(const auto& i : _items) {
        upan::string line;
        if (i.first.empty()) {
          line = i.second + "\n";
        } else {
          line = i.first + "=" + i.second + "\n";
        }
        wstream.write(line.c_str(), line.length());
      }
    }
  }
}
