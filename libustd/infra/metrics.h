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

#include <map.h>
#include <ustring.h>
#include <atomicop.h>
#include <mutex.h>
#include <vector.h>

namespace upan {
  class metrics {
  private:
    static metrics* _instance;

  public:
    class stats {
    public:
      stats();

      void start();
      void end();

      uint32_t count() const;
      uint32_t sum() const;
      double avg() const;

    private:
      uint32_t _tick;
      uint32_t _count;
      uint32_t _sum;
    };

  public:
    static void create();
    static metrics& instance();

    stats& get(const string &name, int id);
    stats& get(const string& name);
    void remove(const string& name);

    uint32_t count(const string& name);
    double avg(const string& name);

    upan::vector<upan::string> kpis();
  private:
    typedef map<int, stats> STAT_MAP;
    typedef map<string, STAT_MAP> STAT_PID_MAP;

    STAT_PID_MAP _stats;
    mutex _mutexStats;
  };
}