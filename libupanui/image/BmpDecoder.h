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

#include <Image.h>
#include <uniq_ptr.h>

namespace upanui {
    class BmpDecoder {
    public:
      BmpDecoder() {}

      typedef struct {
        uint8_t _signature[2];
        uint32_t _fileSize;
        uint32_t _reserved;
        uint32_t _dataOffset;
        void DebugPrint() const {
          printf("\n Signature: %c%c, FileSize: %d, DataOffset: %d", _signature[0], _signature[1], _fileSize, _dataOffset);
        }
      } PACKED Header;

      typedef struct {
        uint32_t _infoHeadersize;
        uint32_t _width;
        uint32_t _height;
        uint16_t _noOfplanes;
        uint16_t _bitsPerPixel;
        uint32_t _compression;
        uint32_t _compressedImageSize;
        uint32_t _xPixelsPerM;
        uint32_t _yPixelsPerM;
        uint32_t _colorsUsed;
        uint32_t _importantColors;
        void DebugPrint() const {
          printf("\n InfoHeaderSize: %d, Width: %d, Height: %d, Planes: %d, BitsPerPixel: %d"
                 "\n Compression: %d, CompressedImgSize: %d"
                 "\n xPixelsPerM: %d, yPixelsPerM: %d "
                 "\n ColorsUsed: %d, ImpColors: %d",
                 _infoHeadersize, _width, _height,
                 _noOfplanes, _bitsPerPixel,
                 _compression, _compressedImageSize,
                 _xPixelsPerM, _yPixelsPerM, _colorsUsed, _importantColors);
        }
      } PACKED InfoHeader;

      Image& parse(const void* imageData, upan::option<uint32_t> transparentColor);
      void DebugPrint() const;

    private:
      Header _header;
      InfoHeader _infoHeader;
    };
}
