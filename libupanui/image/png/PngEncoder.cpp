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
#include <PngEncoder.h>
#include <PngParser.h>

namespace upanui {
  Image& PngEncoder::decode(const ImageResource& imageResource) {
    return decode(imageResource.data(), imageResource.size());
  }

  Image& PngEncoder::decode(const void *imageData, size_t len) {
    upan::uniq_ptr<PngParser> parser(new PngParser(imageData, len));

    size_t out_size;
    parser->decoded_image_size(PngParser::PNG_FMT_RGBA8, out_size);
    byte* out = new byte[out_size];

    parser->decode_image(out, out_size, PngParser::PNG_FMT_RGBA8, 0);

    for(auto i = 0; i < out_size; i += 4) {
      byte x = out[i + 2];
      out[i + 2] = out[i];
      out[i] = x;
    }

    return *new Image(parser->image_width(), parser->image_height(), (uint32_t*)out);
  }
}