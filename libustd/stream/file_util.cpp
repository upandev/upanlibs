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

#include <file_util.h>
#include <stdlib.h>
#include <vector.h>
#include <fs.h>

namespace upan {
  namespace file_path {
    upan::option<upan::string> resolve(const upan::string& fileName, const upan::string& pathEnvVar, const upan::string& defPath) {
      char szPathEnvVal[MAX_ENV_VAL_LEN] = "";
      getenv(pathEnvVar.c_str(), szPathEnvVal);

      upan::string pathEnvVal(szPathEnvVal);
      pathEnvVal += defPath;

      upan::vector<upan::string> pathTokens;
      pathTokens.push_back(".");
      pathEnvVal.tokenize(":", true, pathTokens);

      upan::option<upan::string> res = upan::option<upan::string>::empty();

      for(const auto& path : pathTokens) {
        const auto filePath = path + "/" + fileName;
        if (!access(filePath.c_str(), O_RDONLY)) {
          return upan::option<upan::string>(filePath);
        }
      }
      return upan::option<upan::string>::empty();
    }
  }
}