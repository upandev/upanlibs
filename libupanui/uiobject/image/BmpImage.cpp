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

#include <BmpImage.h>
#include <exception.h>
#include <ColorPalettes.h>

namespace upanui {
  BmpImage::BmpImage(upan::uniq_ptr<uint32_t>&& imageBuffer, const Header& header, const InfoHeader& infoHeader, const int x, const int y)
    : Image(x, y, infoHeader._width, infoHeader._height),
      _imageBuffer(upan::move(imageBuffer)),
      _header(header),
      _infoHeader(infoHeader) {
  }

  BmpImage::~BmpImage() noexcept {
  }

  void BmpImage::DebugPrint() const {
    _header.DebugPrint();
    _infoHeader.DebugPrint();
  }

  uint32_t* BmpImage::parse(const void* imageData, Header& header, InfoHeader& infoHeader, const uint32_t transparentColor) {
    if (imageData == nullptr) {
      throw upan::exception(XLOC, "imageData can't be null");
    }

    memcpy(&header, imageData, sizeof(Header));
    if (header._signature[0] != 'B' || header._signature[1] != 'M') {
      throw upan::exception(XLOC, "invalid BMP image data. Signature = %c%c", header._signature[0], header._signature[1]);
    }

    memcpy(&infoHeader, (void*)((uint32_t)imageData + sizeof(Header)), sizeof(InfoHeader));
    if (infoHeader._infoHeadersize != 40) {
      throw upan::exception(XLOC, "can't support BMP InfoHeader size != 40");
    }

    if (infoHeader._compression != 0) {
      throw upan::exception(XLOC, "compressed BMP is not supported yet (%d)", infoHeader._compression);
    }

    if (infoHeader._width < 1 || infoHeader._width > 3000) {
      throw upan::exception(XLOC, "unsupported width %d", infoHeader._width);
    }

    if (infoHeader._height < 1 || infoHeader._height > 3000) {
      throw upan::exception(XLOC, "unsupported height %d", infoHeader._height);
    }

    if (infoHeader._bitsPerPixel != 4 && infoHeader._bitsPerPixel != 8 && infoHeader._bitsPerPixel != 24) {
      throw upan::exception(XLOC, "unsupported BMP resolution: %d", infoHeader._bitsPerPixel);
    }

    const auto headerSize = sizeof(Header) + sizeof(InfoHeader);
    const auto colorTableSize = header._dataOffset - headerSize;
    //4 bytes per pixel * no. of bits per pixel
    const auto colorTableExists = (infoHeader._bitsPerPixel == 4 && colorTableSize == 16 * 4)
        || (infoHeader._bitsPerPixel == 8 && colorTableSize == 256 * 4);

    //TODO: sort colorTable by _importantColors.
    const uint32_t* colorTable = colorTableExists ? static_cast<const uint32_t*>((void*)((uint32_t)imageData + headerSize))
        : infoHeader._bitsPerPixel == 4 ? ColorPalettes::CP16::GetColorTable()
        : infoHeader._bitsPerPixel == 8 ? ColorPalettes::CP256::GetColorTable()
        : nullptr;

    const auto pixelData = static_cast<const uint8_t*>((void*)((uint32_t)imageData + header._dataOffset));
    const uint32_t imageBufferSize = infoHeader._width * infoHeader._height;
    upan::uniq_ptr<uint32_t> imageBuffer = new uint32_t[imageBufferSize];

    int scanLinePadding = 0;
    switch(infoHeader._bitsPerPixel) {
      case 4: {
        //2 pixels in 1 byte
        int delta = (infoHeader._width / 2) % 4;
        if (delta) {
          scanLinePadding = (4 - delta) * 2;
        }
      }
      break;
      case 8: {
        //1 pixel per byte
        int delta = infoHeader._width % 4;
        if (delta) {
          scanLinePadding = 4 - delta;
        }
      }
      break;
      case 24: {
        //1 pixel = 3 bytes
        int delta = (3 * infoHeader._width) % 4;
        if (delta) {
          scanLinePadding = 4 - delta;
        }
      }
      break;
    }

    auto applyTransparencyFilter = [transparentColor](const uint32_t color) -> uint32_t  {
      return color == transparentColor ? color & 0x00FFFFFF : color | 0xFF000000;
    };

    int dataIndex = 0;
    for(int y = infoHeader._height - 1; y >= 0; --y) {
      const auto y_offset = y * infoHeader._width;
      for(uint32_t x = 0; x < infoHeader._width; ++x) {
        auto p = (uint32_t*)(imageBuffer.get() + y_offset + x);
        switch(infoHeader._bitsPerPixel) {
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
            throw upan::exception(XLOC, "unsupported BMP resolution: %d", infoHeader._bitsPerPixel);
        }
      }
      dataIndex += scanLinePadding;
    }
    return imageBuffer.release();
  }

  BmpImage& BmpImage::create(const void* imageData, const int x, const int y, const uint32_t transparentColor) {
    Header header;
    InfoHeader infoHeader;
    upan::uniq_ptr<uint32_t> imageBuffer(parse(imageData, header, infoHeader, transparentColor));
    return *new BmpImage(upan::move(imageBuffer), header, infoHeader, x, y);
  }

  void BmpImage::draw() {
  }
}
