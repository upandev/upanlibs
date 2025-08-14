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

#include <netdb.h>
#include <file_stream.h>
#include <syslog.h>
#include <vector.h>
#include <mutex.h>
#include <fs.h>

#define SERVICES_FILE_PATH "/etc/services"

class ServentDB {
private:
  ServentDB();

public:
  static ServentDB& instance();
  void load();
  void rewind() { _itIndex = 0; }
  struct servent* getservbyname(const char *name, const char *proto);
  struct servent* getservbyport(int port, const char *proto);
  struct servent* getservent();

private:
  struct timeval _lastModifiedTime;
  int _itIndex;
  upan::mutex _mutex;
  upan::vector<struct servent> _serviceEntries;
};

ServentDB& ServentDB::instance() {
  static ServentDB instance;
  return instance;
}

ServentDB::ServentDB() : _itIndex(0) {
  _lastModifiedTime.tv_sec = 0;
  _lastModifiedTime.tv_usec = 0;
}

void ServentDB::load() {
  upan::mutex_guard g(_mutex);

  struct stat fileStat;
  if (stat(SERVICES_FILE_PATH, &fileStat)) {
    syslog(LOG_ERR, "Failed to get stat %s", SERVICES_FILE_PATH);
    return;
  }

  if (_lastModifiedTime.tv_sec == fileStat.st_mtime.tv_sec && _lastModifiedTime.tv_usec == fileStat.st_mtime.tv_usec) {
    return;
  }

  upan::file_stream servicesFile(SERVICES_FILE_PATH, O_RDONLY);
  if (!servicesFile.is_good()) {
    syslog(LOG_ERR, "%s doesn't exist", SERVICES_FILE_PATH);
    return;
  }

  _lastModifiedTime = fileStat.st_mtime;

  for(auto& e : _serviceEntries) {
    free(e.s_name);
    free(e.s_proto);
    for (int i = 0; e.s_aliases[i] != NULL; ++i) {
      free(e.s_aliases[i]);
    }
    free(e.s_aliases);
  }
  _serviceEntries.clear();

  upan::string line;
  while (servicesFile.read_line(line)) {
    line = upan::string::trim(line);
    if (line[0] != '#') {
      upan::vector<upan::string> tokens;
      line.tokenize(" ", true, tokens);
      if (tokens.size() >= 2) {
        upan::string port, proto;
        if (tokens[1].split('/', port, proto)) {
          struct servent e;
          e.s_name = (char*)malloc(tokens[0].length() + 1);
          strcpy(e.s_name, tokens[0].c_str());

          e.s_port = atoi(port.c_str());
          e.s_proto = (char*)malloc(proto.length() + 1);
          strcpy(e.s_proto, proto.c_str());

          const int aliasCount = tokens.size() - 2;
          e.s_aliases = (char**)malloc((aliasCount + 1) * sizeof(char*));
          for (int i = 0; i < aliasCount; ++i) {
            e.s_aliases[i] = (char*)malloc(tokens[i + 2].length() + 1);
            strcpy(e.s_aliases[i], tokens[i + 2].c_str());
          }
          e.s_aliases[aliasCount] = NULL;

          _serviceEntries.push_back(e);
        }
      }
    }
  }
}

struct servent* ServentDB::getservbyname(const char *name, const char *proto) {
  upan::mutex_guard g(_mutex);
  load();

  for (int i = 0; i < _serviceEntries.size(); ++i) {
    if (strcmp(_serviceEntries[i].s_name, name) == 0 && (proto == NULL || strcmp(_serviceEntries[i].s_proto, proto) == 0)) {
      _itIndex = i;
      return &_serviceEntries[i];
    }
  }

  return NULL;
}

struct servent* ServentDB::getservbyport(int port, const char *proto) {
  upan::mutex_guard g(_mutex);
  load();

  for (int i = 0; i < _serviceEntries.size(); ++i) {
    if (_serviceEntries[i].s_port == port && (proto == NULL || strcmp(_serviceEntries[i].s_proto, proto) == 0)) {
      _itIndex = i;
      return &_serviceEntries[i];
    }
  }

  return NULL;
}

struct servent* ServentDB::getservent() {
  upan::mutex_guard g(_mutex);
  load();

  if (_itIndex < _serviceEntries.size()) {
    return &_serviceEntries[_itIndex++];
  }

  return NULL;
}

struct servent* getservbyname(const char *name, const char *proto) {
  return ServentDB::instance().getservbyname(name, proto);
}

struct servent* getservbyport(int port, const char *proto) {
  return ServentDB::instance().getservbyport(port, proto);
}

struct servent* getservent() {
  return ServentDB::instance().getservent();
}

void setservent(int stayopen) {
  ServentDB::instance().rewind();
}

void endservent() {
  ServentDB::instance().rewind();
}