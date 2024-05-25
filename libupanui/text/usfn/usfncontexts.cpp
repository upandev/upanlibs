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

#include <usfncontexts.h>

namespace upanui {
  namespace usfn {
    Contexts::~Contexts() {
      for(auto i : _fontContexts) {
        delete i.second;
      }
    }

    Context& Contexts::get(usfn::PreloadedFonts fontType, uint8_t fontSize, uint16_t fontStyle) {
      const uint64_t fontContextType = fontType | fontSize << 8 | fontStyle << 16;
      auto i = _fontContexts.find(fontContextType);
      if (i != _fontContexts.end()) {
        return *(i->second);
      }
      auto context = new Context();
      context->Load(upanui::usfn::Context::GetPreloadedFont(fontType));
      context->Select(upanui::usfn::FAMILY_ANY, nullptr, fontStyle, fontSize);
      _fontContexts.insert(FontContextMap::value_type(fontContextType, context));
      return *context;
    }
  }
}