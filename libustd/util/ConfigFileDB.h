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

#include <pair.h>
#include <list.h>

namespace upan {
  class ConfigFileDB {
  public:
    enum OpType {
      RDONLY,
      RDWR
    };

    class BatchWriteGuard {
    public:
      explicit BatchWriteGuard(ConfigFileDB& c) :_configFileDB(c) { c._batchWrite = true; }
      ~BatchWriteGuard() {
        _configFileDB._batchWrite = false;
        _configFileDB.writeToConfigFile();
      }
    private:
      ConfigFileDB& _configFileDB;
    };

    typedef upan::pair<upan::string, upan::string> Item;
    typedef upan::list<Item> Items;

    ConfigFileDB(const upan::string& filePath, OpType opType);
    ConfigFileDB(ConfigFileDB&) = delete;
    ConfigFileDB& operator=(ConfigFileDB&) = delete;

    upan::option<upan::string> get(const upan::string& key);
    void set(const upan::string& key, const upan::string& value, const upan::string& comment);
    void remove(const upan::string& key);

    const Items& getAll() const { return _items; }

  private:
    void writeToConfigFile();

  private:
    upan::string _filePath;
    OpType _opType;
    bool _batchWrite;
    Items _items;
  };
}
