#include <CircularLayout.h>
#include <UIObject.h>
#include <DrawBuffer.h>
#include <GCoreFunctions.h>
#include <math.h>

namespace upanui {
  Layout::BoundaryCheckResult CircularLayout::checkBoundary(UIObject &child) {
    const auto cx1 = child.x();
    const auto cy1 = child.y();
    const auto cx2 = cx1 + child.width() - 1;
    const auto cy2 = cy1 + child.height() - 1;

    const int outerRadius = (parent().width() / 2) - 1;
    const int innerRadius = outerRadius - parent().borderThickness();

    const int cx = parent().width() / 2 - 1;
    const int cy = cx;

    const int dx1 = cx - cx1;
    const int dy1 = cy - cy1;
    const int d1 = sqrt(dx1 * dx1 + dy1 * dy1);

    if (d1 <= innerRadius) {
      const int dx2 = cx - cx2;
      const int dy2 = cy - cy2;
      const int d2 = sqrt(dx2 * dx2 + dy2 * dy2);
      if (d2 <= innerRadius) {
        return BoundaryCheckResult::Inside;
      }
    }

    //TODO: support for PartiallyInside scenario
    return BoundaryCheckResult::Outside;
  }

  void CircularLayout::draw(UIObject& child) {
    // only support child objects that's Inside parent
  }

  void CircularLayout::plotLine(int x, int y, int r, int shift, uint32_t color, GCoreFunctions::PixelCache& pixelCache) {
    const DrawBuffer& drawBuffer = parent().drawBuffer();

    int sx1 = -x + r + shift;
    int sx2 = x + r + shift;
    int sy = r - y + shift;
    int yoffset = sy * drawBuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(drawBuffer.at(i + yoffset), color, pixelCache, drawBuffer.isLocal());
    }

    sy = r + y + shift;
    yoffset = sy * drawBuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(drawBuffer.at(i + yoffset), color, pixelCache, drawBuffer.isLocal());
    }

    sy = x + r + shift;
    sx1 = r - y + shift;
    sx2 = r + y + shift;
    yoffset = sy * drawBuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(drawBuffer.at(i + yoffset), color, pixelCache, drawBuffer.isLocal());
    }

    sy = -x + r + shift;
    yoffset = sy * drawBuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(drawBuffer.at(i + yoffset), color, pixelCache, drawBuffer.isLocal());
    }
  }

  void CircularLayout::plotPixel(int x, int y, int r, int shift, uint32_t color, GCoreFunctions::PixelCache& pixelCache, bool directSet) {
    const DrawBuffer& drawBuffer = parent().drawBuffer();

    int sx1 = -x + r + shift;
    int sx2 = x + r + shift;
    int sy = r - y + shift;
    int yoffset = sy * drawBuffer.width();
    GCoreFunctions::setPixel(drawBuffer.at(sx1 + yoffset), color, pixelCache, directSet);
    GCoreFunctions::setPixel(drawBuffer.at(sx2 + yoffset), color, pixelCache, directSet);

    sy = r + y + shift;
    yoffset = sy * drawBuffer.width();
    GCoreFunctions::setPixel(drawBuffer.at(sx1 + yoffset), color, pixelCache, directSet);
    GCoreFunctions::setPixel(drawBuffer.at(sx2 + yoffset), color, pixelCache, directSet);

    sy = x + r + shift;
    sx1 = r - y + shift;
    sx2 = r + y + shift;
    yoffset = sy * drawBuffer.width();
    GCoreFunctions::setPixel(drawBuffer.at(sx1 + yoffset), color, pixelCache, directSet);
    GCoreFunctions::setPixel(drawBuffer.at(sx2 + yoffset), color, pixelCache, directSet);

    sy = -x + r + shift;
    yoffset = sy * drawBuffer.width();
    GCoreFunctions::setPixel(drawBuffer.at(sx1 + yoffset), color, pixelCache, directSet);
    GCoreFunctions::setPixel(drawBuffer.at(sx2 + yoffset), color, pixelCache, directSet);
  }

  void CircularLayout::fill(uint32_t alpha, uint32_t rawColor, int shift, int r) {
    if (alpha == 0 && parent().borderThickness() == 0) {
      return;
    }
    rawColor &= GCoreFunctions::NO_ALPHA_MASK;
    const auto color = rawColor | (alpha << 24);

    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    const int r2 = r * r;
    float e = 0;
    GCoreFunctions::PixelCache pixelCache;

    while (x <= y) {
      plotLine(x, y, r, shift, color, pixelCache);

      if (d < 0) {
        d += 4 * x + 6;
      } else {
        d += 4 * (x - y) + 10;
        //we are going to calculate actual y as part of anti-aliasing code below
        //--y;
      }
      ++x;

      float ya = sqrt(r2 - x * x);
      //because the value of y is positive for the calculated quadrant, it's ok to truncate ya instead of floor;
      //y = floor(ya);
      y = ya;
      e = ya - y;

      if (e > 0 && e < 1) {
        const uint32_t alpha_e = alpha * e;
        const auto antialiasPixelColor = (rawColor & 0xFFFFFF) | alpha_e << 24;
        //shift == 0 => outer/border circle => do direct-set => no alpha blending
        if (shift == 0) {
          plotPixel(x, y + 1, r, shift, antialiasPixelColor, pixelCache, parent().drawBuffer().isLocal());
        } else {
          const int sx1 = -x + r + shift;
          const int sy = r - y + shift;
          auto cpixel = parent().drawBuffer().at(sx1, sy);
          const uint32_t calpha_e = (cpixel >> 24) * (1 - e) + 1;
          //cpixel = (cpixel & 0xFFFFFF) | calpha_e << 24;
          GCoreFunctions::setPixel(cpixel, antialiasPixelColor, pixelCache, false);
          cpixel = (cpixel & 0xFFFFFF) | (calpha_e + alpha_e) << 24;
          plotPixel(x, y + 1, r, shift, cpixel, pixelCache, true);
        }
      }
    }
  }

  void CircularLayout::fill() {
    //TODO: optimize by caching drawbuffer
    const int outerCircleRadius = (parent().width() / 2) - 1;
    const int innerCircleRadius = outerCircleRadius - parent().borderThickness();
    if (outerCircleRadius > innerCircleRadius) {
      fill(parent().borderColorAlpha(), parent().borderColor(), 0, outerCircleRadius);
    }
    fill(parent().backgroundColorAlpha(), parent().backgroundColorForDraw(), parent().borderThickness(), innerCircleRadius);
  }
}