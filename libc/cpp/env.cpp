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

#include <stdlib.h>
#include <map.h>
#include <ustring.h>
#include <uniq_ptr.h>
#include <mutex.h>
#include <dtime.h>

class Env {
public:
  static Env& instance() {
    static Env _instance;
    return _instance;
  }

  Env() : _environUpdatedTime(0), _envModifiedTime(btime()), _environ(nullptr) {}
  ~Env() { freeEnviron(); }

  void freeEnviron();
  void load(const char** environ);
  const char* get(const char* key);
  int set(const char* key, const char* value, int overwrite);
  void unset(const char* key);
  void clear();
  const char** environ();

private:
  typedef upan::map<upan::string, upan::string> ENV_MAP;
  time_t _environUpdatedTime;
  time_t _envModifiedTime;
  char** _environ;
  ENV_MAP _envMap;
  upan::mutex _mutex;
};

void Env::load(const char** environ) {
  if (environ) {
    for (int i = 0; environ[i]; ++i) {
      putenv(environ[i]);
    }
  }
}

const char* Env::get(const char* key) {
  upan::mutex_guard g(_mutex);
  auto i = _envMap.find(key);
  if (i == _envMap.end()) {
    return nullptr;
  } else {
    return i->second.c_str();
  }
}

int Env::set(const char* key, const char* value, int overwrite) {
  upan::mutex_guard g(_mutex);
  if (key == nullptr) { return -1; }
  if (strlen(key) == 0) { return -1; }
  if (strchr(key, '=') != nullptr) { return -1; }

  auto i = _envMap.find(key);
  if (i != _envMap.end()) {
    if (overwrite) {
      i->second = value;
    } else {
      return 0;
    }
  }

  _envMap.insert(ENV_MAP::value_type(key, value));
  _envModifiedTime = btime();
  return 0;
}

void Env::unset(const char* key) {
  upan::mutex_guard g(_mutex);
  if (_envMap.erase(key)) {
    _envModifiedTime = btime();
  }
}

void Env::freeEnviron() {
  if (_environ) {
    for(int i = 0; _environ[i]; ++i) {
      free(_environ[i]);
    }
    free(_environ);
  }
}

void Env::clear() {
  upan::mutex_guard g(_mutex);
  freeEnviron();
  _environ = nullptr;
  _envMap.clear();
  _envModifiedTime = btime();
}

const char** Env::environ() {
  upan::mutex_guard g(_mutex);
  if (_environ == nullptr || _environUpdatedTime != _envModifiedTime) {
    freeEnviron();
    _environ = (char**)malloc(_envMap.size() + 1);
    int i = 0;
    for(const auto& e : _envMap) {
      _environ[i] = (char*)malloc(e.first.length() + e.second.length() + 2);
      sprintf(_environ[i], "%s=%s", e.first.c_str(), e.second.c_str());
      ++i;
    }
    _environ[i] = nullptr;
    _environUpdatedTime = _envModifiedTime;
  }

  return (const char**)_environ;
}

const char* getenv(const char* var) {
  return Env::instance().get(var);
}

#if defined __cplusplus
extern "C" {
#endif
void load_environ(const char** environ) {
  Env::instance().load(environ);
}
#if defined __cplusplus
}
#endif

int setenv(const char* key, const char* val, int overwrite) {
  return Env::instance().set(key, val, overwrite);
}

int putenv(const char* env) {
  if (env == nullptr) { return -1; }

  char* delim = strchr (env, '=');
  if (delim != nullptr) {
    *delim = '\0';
    return setenv(env, delim + 1, 1);
  }
  return -1;
}

int unsetenv(const char* key) {
  if (key == nullptr) { return -1; }
  if (strlen(key) == 0) { return -1; }
  if (strchr(key, '=') != nullptr) { return -1; }
  Env::instance().unset(key);
  return 0;
}

int clearenv() {
  Env::instance().clear();
  return 0;
}

const char** getenviron() {
  return Env::instance().environ();
}
