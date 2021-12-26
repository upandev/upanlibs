#include <CircularLayout.h>
#include <UIObject.h>
#include <DrawBuffer.h>
#include <GCoreFunctions.h>
#include <math.h>

namespace upanui {
  Layout::BoundaryCheckResult CircularLayout::checkBoundary(UIObject &child) {
    return Layout::Outside;
  }

  void CircularLayout::draw(UIObject& child) {
  }

  void CircularLayout::plotLine(int x, int y, int r, uint32_t color) {
    const DrawBuffer& drawBuffer = parent().drawBuffer();

    int sx1 = -x + r;
    int sx2 = x + r;
    int sy = r - y;
    int yoffset = sy * drawBuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(drawBuffer.buffer()[i + yoffset], color, drawBuffer.isLocal());
    }

    sy = r + y;
    yoffset = sy * drawBuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(drawBuffer.buffer()[i + yoffset], color, drawBuffer.isLocal());
    }

    sy = x + r;
    sx1 = r - y;
    sx2 = r + y;
    yoffset = sy * drawBuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(drawBuffer.buffer()[i + yoffset], color, drawBuffer.isLocal());
    }

    sy = -x + r;
    yoffset = sy * drawBuffer.width();
    for (int i = sx1; i <= sx2; ++i) {
      GCoreFunctions::setPixel(drawBuffer.buffer()[i + yoffset], color, drawBuffer.isLocal());
    }
  }

  void CircularLayout::plotPixel(int x, int y, int r, uint32_t color) {
    const DrawBuffer& drawBuffer = parent().drawBuffer();

    int sx1 = -x + r;
    int sx2 = x + r;
    int sy = r - y;
    int yoffset = sy * drawBuffer.width();
    GCoreFunctions::setPixel(drawBuffer.buffer()[sx1 + yoffset], color, drawBuffer.isLocal());
    GCoreFunctions::setPixel(drawBuffer.buffer()[sx2 + yoffset], color, drawBuffer.isLocal());

    sy = r + y;
    yoffset = sy * drawBuffer.width();
    GCoreFunctions::setPixel(drawBuffer.buffer()[sx1 + yoffset], color, drawBuffer.isLocal());
    GCoreFunctions::setPixel(drawBuffer.buffer()[sx2 + yoffset], color, drawBuffer.isLocal());

    sy = x + r;
    sx1 = r - y;
    sx2 = r + y;
    yoffset = sy * drawBuffer.width();
    GCoreFunctions::setPixel(drawBuffer.buffer()[sx1 + yoffset], color, drawBuffer.isLocal());
    GCoreFunctions::setPixel(drawBuffer.buffer()[sx2 + yoffset], color, drawBuffer.isLocal());

    sy = -x + r;
    yoffset = sy * drawBuffer.width();
    GCoreFunctions::setPixel(drawBuffer.buffer()[sx1 + yoffset], color, drawBuffer.isLocal());
    GCoreFunctions::setPixel(drawBuffer.buffer()[sx2 + yoffset], color, drawBuffer.isLocal());
  }

  void CircularLayout::fill() {
    const auto alpha = parent().backgroundColorAlpha();
    if (alpha == 0) {
      return;
    }

    const auto rawColor = (parent().backgroundColorForDraw() & ~GCoreFunctions::ALPHA_MASK);
    const auto color = rawColor | (alpha << 24);

    const int r = parent().width() / 2;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    const int r2 = r * r;

    while (x <= y) {
      plotLine(x, y, r, color);

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
      auto e = ya - y;

      if (e > 0 && e < 1) {
        const auto antialiasPixelColor = rawColor | ((int) (alpha * e)) << 24;
        plotPixel(x, y + 1, r, antialiasPixelColor);
      }
    }
  }
}