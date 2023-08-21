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

#include <BmpEncoder.h>
#include <exception.h>
#include <ColorPalettes.h>
#include <GCoreFunctions.h>

namespace upanui {
  void BmpEncoder::DebugPrint() const {
    _header.DebugPrint();
    _infoHeader.DebugPrint();
  }

  Image& BmpEncoder::decode(const ImageResource& imageResource, upan::option<uint32_t> transparentColor) {
    return decode(imageResource.data(), transparentColor);
  }

  Image& BmpEncoder::decode(const void* imageData, upan::option<uint32_t> transparentColor) {
    if (imageData == nullptr) {
      throw upan::exception(XLOC, "imageData can't be null");
    }

    memcpy((void*)&_header, imageData, sizeof(Header));
    if (_header._signature[0] != 'B' || _header._signature[1] != 'M') {
      throw upan::exception(XLOC, "invalid BMP image data. Signature = %c%c", _header._signature[0], _header._signature[1]);
    }

    memcpy((void*)&_infoHeader, (void*)((uintptr_t)imageData + sizeof(Header)), sizeof(InfoHeader));
    if (_infoHeader._infoHeadersize != 40) {
      throw upan::exception(XLOC, "can't support BMP InfoHeader size != 40");
    }

    if (_infoHeader._compression != 0) {
      throw upan::exception(XLOC, "compressed BMP is not supported yet (%d)", _infoHeader._compression);
    }

    if (_infoHeader._width < 1 || _infoHeader._width > 3000) {
      throw upan::exception(XLOC, "unsupported width %d", _infoHeader._width);
    }

    if (_infoHeader._height < 1 || _infoHeader._height > 3000) {
      throw upan::exception(XLOC, "unsupported height %d", _infoHeader._height);
    }

    if (_infoHeader._bitsPerPixel != 4 && _infoHeader._bitsPerPixel != 8 && _infoHeader._bitsPerPixel != 24) {
      throw upan::exception(XLOC, "unsupported BMP resolution: %d", _infoHeader._bitsPerPixel);
    }

    const auto headerSize = sizeof(Header) + sizeof(InfoHeader);
    const auto colorTableSize = _header._dataOffset - headerSize;
    //4 bytes per pixel * no. of bits per pixel
    const auto colorTableExists = (_infoHeader._bitsPerPixel == 4 && colorTableSize == 16 * 4)
        || (_infoHeader._bitsPerPixel == 8 && colorTableSize == 256 * 4);

    //TODO: sort colorTable by _importantColors.
    const uint32_t* colorTable = colorTableExists ? static_cast<const uint32_t*>((void*)((uintptr_t)imageData + headerSize))
        : _infoHeader._bitsPerPixel == 4 ? ColorPalettes::CP16::GetColorTable()
        : _infoHeader._bitsPerPixel == 8 ? ColorPalettes::CP256::GetColorTable()
        : nullptr;

    const auto pixelData = static_cast<const uint8_t*>((void*)((uintptr_t)imageData + _header._dataOffset));
    const uint32_t imageBufferSize = _infoHeader._width * _infoHeader._height;
    upan::uniq_ptr<uint32_t> imageBuffer = new uint32_t[imageBufferSize];

    int scanLinePadding = 0;
    switch(_infoHeader._bitsPerPixel) {
      case 4: {
        //2 pixels in 1 byte
        int delta = (_infoHeader._width / 2) % 4;
        if (delta) {
          scanLinePadding = (4 - delta) * 2;
        }
      }
      break;
      case 8: {
        //1 pixel per byte
        int delta = _infoHeader._width % 4;
        if (delta) {
          scanLinePadding = 4 - delta;
        }
      }
      break;
      case 24: {
        //1 pixel = 3 bytes
        int delta = (3 * _infoHeader._width) % 4;
        if (delta) {
          scanLinePadding = 4 - delta;
        }
      }
      break;
    }

    auto applyTransparencyFilter = [transparentColor](const uint32_t color) -> uint32_t  {
      if (!transparentColor.isEmpty() && transparentColor.value() == color) {
        return color & GCoreFunctions::NO_ALPHA_MASK;
      } else {
        return color | GCoreFunctions::ALPHA_MASK;
      }
    };

    int dataIndex = 0;
    for(int y = _infoHeader._height - 1; y >= 0; --y) {
      const auto y_offset = y * _infoHeader._width;
      for(uint32_t x = 0; x < _infoHeader._width; ++x) {
        auto p = (uint32_t*)(imageBuffer.get() + y_offset + x);
        switch(_infoHeader._bitsPerPixel) {
          case 4: {
            const uint8_t code = pixelData[dataIndex / 2];
            const uint32_t colorCode = dataIndex & 0x1 ? code & 0xF : (code >> 4) & 0xF;
            *p = applyTransparencyFilter(colorTable[colorCode]);
            ++dataIndex;
          }
          break;

          case 8: {
            const auto colorCode = pixelData[dataIndex] & 0xFF;
            *p = applyTransparencyFilter(colorTable[colorCode]);
            ++dataIndex;
          }
          break;

          case 24: {
            const uint32_t b = pixelData[dataIndex++] & 0xFF;
            const uint32_t g = pixelData[dataIndex++] & 0xFF;
            const uint32_t r = pixelData[dataIndex++] & 0xFF;
            const uint32_t color = ((r << 16) & 0x00FF0000) | ((g << 8) & 0x0000FF00) | (b & 0x000000FF);
            *p = applyTransparencyFilter(color);
          }
          break;

          default:
            throw upan::exception(XLOC, "unsupported BMP resolution: %d", _infoHeader._bitsPerPixel);
        }
      }
      dataIndex += scanLinePadding;
    }
    return *new Image(_infoHeader._width, _infoHeader._height, imageBuffer.release());
  }
}
