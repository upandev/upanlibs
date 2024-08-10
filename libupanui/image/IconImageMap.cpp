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
#include <IconImageMap.h>
#include <Image.h>
#include <file_stream.h>
#include <list.h>
#include <PngEncoder.h>

namespace upanui {
  IconImageMap& IconImageMap::Instance() {
    static IconImageMap _instance;
    return _instance;
  }

  IconImageMap::IconImageMap() :
    TYPE_DIR("dir"),
    TYPE_UNKNOWN("unknown"),
    TYPE_DEFAULT_EXE("exe"),
    TYPE_DEFAULT_FILE("file") {
    upan::file_stream rstream("/icons/imap.cfg", O_RDONLY);

    ImageMap* _curImageMap = nullptr;
    upan::string line;
    while (rstream.read_line(line)) {
      if (line == "[name_map]") {
        _curImageMap = &_nameMap;
      } else if (line == "[ext_map]") {
        _curImageMap = &_extensionMap;
      } else if (line == "[type_map]") {
        _curImageMap = &_typeMap;
      } else if (_curImageMap) {
        upan::string key, value;
        if(line.split('=', key, value)) {
          ImageEntry entry(value, nullptr);
          _curImageMap->insert(ImageMap::value_type(key, entry));
        }
      }
    }
//    for(auto& e : _nameMap) {
//      printf("name -> %s:%s\n", e.first.c_str(), e.second.first.c_str());
//    }
//    for(auto& e : _extensionMap) {
//      printf("ext -> %s:%s\n", e.first.c_str(), e.second.first.c_str());
//    }
//    for(auto& e : _typeMap) {
//      printf("type -> %s:%s\n", e.first.c_str(), e.second.first.c_str());
//    }
  }

  Image& IconImageMap::find(const upan::string& fileName, uint16_t fileAttr) {
    ImageEntry* entry = nullptr;
    if (S_ISDIR(fileAttr)) {
      auto i = _typeMap.find(TYPE_DIR);
      if (i != _typeMap.end()) {
        entry = &i->second;
      }
    } else {
      auto i = _nameMap.find(fileName);
      if (i != _nameMap.end()) {
        entry = &i->second;
      } else {
        upan::list<upan::string> tokens;
        fileName.tokenize(".", true, tokens);
        if (tokens.size() > 1) {
          auto ei = _extensionMap.find(tokens.back());
          if (ei != _extensionMap.end()) {
            entry = &ei->second;
          }
        }
      }
    }

    if (!entry) {
      if (HAS_EXE_PERM(G_OWNER(fileAttr))) {
        auto i = _typeMap.find(TYPE_DEFAULT_EXE);
        if (i != _typeMap.end()) {
          entry = &i->second;
        }
      } else {
        auto i = _typeMap.find(TYPE_DEFAULT_FILE);
        if (i != _typeMap.end()) {
          entry = &i->second;
        }
      }
    }

    if (!entry) {
      auto i = _typeMap.find(TYPE_UNKNOWN);
      if (i == _typeMap.end()) {
        throw upan::exception(XLOC, "unable to find icon image for file: %s", fileName.c_str());
      }
      entry = &i->second;
    }

    if (entry->second == nullptr) {
      auto fd = open(entry->first.c_str(), O_RDONLY);
      if (fd < 0) {
        throw upan::exception(XLOC, "invalid icon image file: %s", entry->first.c_str());
      }

      struct stat s;
      if(fstat(fd, &s)) {
        throw upan::exception(XLOC, "failed to get stats for icon image file: %s", entry->first.c_str());
      }

      upan::uniq_ptr<uint8_t[]> buffer(new uint8_t[s.st_size]);
      read(fd, buffer.get(), s.st_size);

      upanui::PngEncoder decoder;
      upanui::Image& image = decoder.decode(buffer.get(), s.st_size);
      entry->second = &image;

      close(fd);
    }

    return *entry->second;
  }
}
