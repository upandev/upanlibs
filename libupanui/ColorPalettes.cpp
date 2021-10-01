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

#include <exception.h>
#include <ColorPalettes.h>

static const uint32_t COLOR_PALETTE_16[] = {
    0x000000, //BLACK
    0x800000, //MAROON
    0x008000, //GREEN
    0x808000, //OLIVE
    0x000080, //NAVY
    0x800080, //PURPLE
    0x008080, //TEAL
    0xC0C0C0, //SILVER
    0x808080, //GRAY
    0xFF0000, //RED
    0x00FF00, //LIME
    0xFFFF00, //YELLOW
    0x0000FF, //BLUE
    0xFFC0CB, //PINK
    0x00FFFF, //AQUA
    0xFFFFFF //WHITE
};

uint32_t ColorPalettes::CP16::Get(int index) {
  if (index < 0 || index > 15) {
    throw upan::exception(XLOC, "Invalid color code %d for 16 color pallete", index);
  }
  return COLOR_PALETTE_16[index];
}

const uint32_t* ColorPalettes::CP16::GetColorTable() {
  return COLOR_PALETTE_16;
}

static const uint32_t COLOR_PALETTE_256[] = {
    0x000000,
    0x800000,
    0x008000,
    0x808000,
    0x000080,
    0x800080,
    0x008080,
    0xc0c0c0,
    0xc0dcc0,
    0xa6caf0,
    0x2a3faa,
    0x2a3fff,
    0x2a5f00,
    0x2a5f55,
    0x2a5faa,
    0x2a5fff,
    0x2a7f00,
    0x2a7f55,
    0x2a7faa,
    0x2a7fff,
    0x2a9f00,
    0x2a9f55,
    0x2a9faa,
    0x2a9fff,
    0x2abf00,
    0x2abf55,
    0x2abfaa,
    0x2abfff,
    0x2adf00,
    0x2adf55,
    0x2adfaa,
    0x2adfff,
    0x2aff00,
    0x2aff55,
    0x2affaa,
    0x2affff,
    0x550000,
    0x550055,
    0x5500aa,
    0x5500ff,
    0x551f00,
    0x551f55,
    0x551faa,
    0x551fff,
    0x553f00,
    0x553f55,
    0x553faa,
    0x553fff,
    0x555f00,
    0x555f55,
    0x555faa,
    0x555fff,
    0x557f00,
    0x557f55,
    0x557faa,
    0x557fff,
    0x559f00,
    0x559f55,
    0x559faa,
    0x559fff,
    0x55bf00,
    0x55bf55,
    0x55bfaa,
    0x55bfff,
    0x55df00,
    0x55df55,
    0x55dfaa,
    0x55dfff,
    0x55ff00,
    0x55ff55,
    0x55ffaa,
    0x55ffff,
    0x7f0000,
    0x7f0055,
    0x7f00aa,
    0x7f00ff,
    0x7f1f00,
    0x7f1f55,
    0x7f1faa,
    0x7f1fff,
    0x7f3f00,
    0x7f3f55,
    0x7f3faa,
    0x7f3fff,
    0x7f5f00,
    0x7f5f55,
    0x7f5faa,
    0x7f5fff,
    0x7f7f00,
    0x7f7f55,
    0x7f7faa,
    0x7f7fff,
    0x7f9f00,
    0x7f9f55,
    0x7f9faa,
    0x7f9fff,
    0x7fbf00,
    0x7fbf55,
    0x7fbfaa,
    0x7fbfff,
    0x7fdf00,
    0x7fdf55,
    0x7fdfaa,
    0x7fdfff,
    0x7fff00,
    0x7fff55,
    0x7fffaa,
    0x7fffff,
    0xaa0000,
    0xaa0055,
    0xaa00aa,
    0xaa00ff,
    0xaa1f00,
    0xaa1f55,
    0xaa1faa,
    0xaa1fff,
    0xaa3f00,
    0xaa3f55,
    0xaa3faa,
    0xaa3fff,
    0xaa5f00,
    0xaa5f55,
    0xaa5faa,
    0xaa5fff,
    0xaa7f00,
    0xaa7f55,
    0xaa7faa,
    0xaa7fff,
    0xaa9f00,
    0xaa9f55,
    0xaa9faa,
    0xaa9fff,
    0xaabf00,
    0xaabf55,
    0xaabfaa,
    0xaabfff,
    0xaadf00,
    0xaadf55,
    0xaadfaa,
    0xaadfff,
    0xaaff00,
    0xaaff55,
    0xaaffaa,
    0xaaffff,
    0xd40000,
    0xd40055,
    0xd400aa,
    0xd400ff,
    0xd41f00,
    0xd41f55,
    0xd41faa,
    0xd41fff,
    0xd43f00,
    0xd43f55,
    0xd43faa,
    0xd43fff,
    0xd45f00,
    0xd45f55,
    0xd45faa,
    0xd45fff,
    0xd47f00,
    0xd47f55,
    0xd47faa,
    0xd47fff,
    0xd49f00,
    0xd49f55,
    0xd49faa,
    0xd49fff,
    0xd4bf00,
    0xd4bf55,
    0xd4bfaa,
    0xd4bfff,
    0xd4df00,
    0xd4df55,
    0xd4dfaa,
    0xd4dfff,
    0xd4ff00,
    0xd4ff55,
    0xd4ffaa,
    0xd4ffff,
    0xff0055,
    0xff00aa,
    0xff1f00,
    0xff1f55,
    0xff1faa,
    0xff1fff,
    0xff3f00,
    0xff3f55,
    0xff3faa,
    0xff3fff,
    0xff5f00,
    0xff5f55,
    0xff5faa,
    0xff5fff,
    0xff7f00,
    0xff7f55,
    0xff7faa,
    0xff7fff,
    0xff9f00,
    0xff9f55,
    0xff9faa,
    0xff9fff,
    0xffbf00,
    0xffbf55,
    0xffbfaa,
    0xffbfff,
    0xffdf00,
    0xffdf55,
    0xffdfaa,
    0xffdfff,
    0xffff55,
    0xffffaa,
    0xccccff,
    0xffccff,
    0x33ffff,
    0x66ffff,
    0x99ffff,
    0xccffff,
    0x007f00,
    0x007f55,
    0x007faa,
    0x007fff,
    0x009f00,
    0x009f55,
    0x009faa,
    0x009fff,
    0x00bf00,
    0x00bf55,
    0x00bfaa,
    0x00bfff,
    0x00df00,
    0x00df55,
    0x00dfaa,
    0x00dfff,
    0x00ff55,
    0x00ffaa,
    0x2a0000,
    0x2a0055,
    0x2a00aa,
    0x2a00ff,
    0x2a1f00,
    0x2a1f55,
    0x2a1faa,
    0x2a1fff,
    0x2a3f00,
    0x2a3f55,
    0xfffbf0,
    0xa0a0a4,
    0x808080,
    0xff0000,
    0x00ff00,
    0xffff00,
    0x0000ff,
    0xff00ff,
    0x00ffff,
    0xffffff
};

uint32_t ColorPalettes::CP256::Get(int index) {
  if (index < 0 || index > 255) {
    throw upan::exception(XLOC, "Invalid color code %d for 256 color pallete", index);
  }
  return COLOR_PALETTE_256[index];
}

const uint32_t* ColorPalettes::CP256::GetColorTable() {
  return COLOR_PALETTE_256;
}
