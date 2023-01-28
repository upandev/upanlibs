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

#include <RectangularLayout.h>
#include <UIObject.h>
#include <DrawBuffer.h>
#include <GCoreFunctions.h>

namespace upanui {
  Layout::BoundaryCheckResult RectangularLayout::checkBoundary(UIObject &child) {
    const auto cx1 = child.x();
    const auto cy1 = child.y();
    const auto cx2 = cx1 + child.width() - 1;
    const auto cy2 = cy1 + child.height() - 1;

    const int pwidth = parent().width() - 2 * parent().borderThickness();
    const int pheight = parent().height() - 2 * parent().borderThickness();

    if (cx1 >= 0 && cx2 < pwidth && cy1 >= 0 && cy2 < pheight) {
      return BoundaryCheckResult::Inside;
    }

    if ((cx1 < 0 && cx2 < 0) ||
        (cy1 < 0 && cy2 < 0) ||
        (cx1 >= pwidth && cx2 >= pwidth) ||
        (cy1 >= pheight && cy2 >= pheight)) {
      return BoundaryCheckResult::Outside;
    }

    return BoundaryCheckResult::PartiallyInside;
  }

  void RectangularLayout::draw(UIObject& child) {
    auto& childDrawBuffer = child.drawBuffer();
    auto& parentDrawBuffer = parent().drawBuffer();

    if (!childDrawBuffer.isLocal()) {
      return;
    }

    const auto pwidth = parent().width() - parent().borderThickness();
    const auto pheight = parent().height() - parent().borderThickness();

    const int sx1 = child.x() >= 0 ? 0 : 0 - child.x();
    const int sy1 = child.y() >= 0 ? 0 : 0 - child.y();

    const int dx1 = (child.x() >= 0 ? child.x() : 0) + parent().borderThickness();
    const int dy1 = (child.y() >= 0 ? child.y() : 0) + parent().borderThickness();

    const int w = (child.width() - sx1) <= (pwidth - dx1) ? child.width() - sx1 : pwidth - dx1;
    const int h = (child.height() - sy1) <= (pheight - dy1) ? child.height() - sy1 : pheight - dy1;

    //printf("\n%d, %d, %d, %d, %d, %d, %d, %d, %d\n", srcX1, srcY1, destX1, destY1, w, h, srcXOffset, destXOffset, copyWidth);
    GCoreFunctions::PixelCache pixelCache;
    for(int y = 0; y < h; ++y) {
      int srcOffet = sx1 + (sy1 + y) * childDrawBuffer.width();
      int destOffset = dx1 + (dy1 + y) * parentDrawBuffer.width();
      for(int x = 0; x < w; ++x) {
        GCoreFunctions::setPixel(parentDrawBuffer.at(x + destOffset), childDrawBuffer.at(x + srcOffet), pixelCache, false);
      }
    }
  }

  void RectangularLayout::fill() {
    const auto alpha = parent().backgroundColorAlpha();
    if (alpha == 0 && parent().borderThickness() == 0) {
      if (parent().drawBuffer().isLocal()) {
        parent().drawBuffer().fill(0);
      }
      return;
    }

    const auto& drawBuffer = parent().drawBuffer();
    const auto bgColor = (parent().backgroundColorForDraw() & ~GCoreFunctions::ALPHA_MASK) | (alpha << 24);
    const auto brColor = (parent().borderColor() & ~GCoreFunctions::ALPHA_MASK) | (parent().borderColorAlpha() << 24);
    GCoreFunctions::PixelCache pixelCache;

    for(auto y = 0u; y < parent().height(); ++y) {
      auto yOffset = y * drawBuffer.width();
      if (y < parent().borderThickness() || (parent().height() - y) <= parent().borderThickness()) {
        for(auto x = 0u; x < parent().width(); ++x) {
          GCoreFunctions::setPixel(drawBuffer.at(x + yOffset), brColor, pixelCache, drawBuffer.isLocal());
        }
      } else {
        for(auto x = 0u; x < parent().borderThickness(); ++x) {
          GCoreFunctions::setPixel(drawBuffer.at(x + yOffset), brColor, pixelCache, drawBuffer.isLocal());
        }
        for(auto x = parent().borderThickness(); x < parent().width() - parent().borderThickness(); ++x) {
          GCoreFunctions::setPixel(drawBuffer.at(x + yOffset), bgColor, pixelCache, drawBuffer.isLocal());
        }
        for(auto x = (parent().width() - parent().borderThickness()); x < parent().width(); ++x) {
          GCoreFunctions::setPixel(drawBuffer.at(x + yOffset), brColor, pixelCache, drawBuffer.isLocal());
        }
      }
    }
  }
}