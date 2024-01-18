#ifndef INVADINGTATERS_COLOR4F_H
#define INVADINGTATERS_COLOR4F_H

#include "color4f.h"
;

class Color3: public Color4 {
  Color3(float red, float green, float blue): Color4(red, green, blue, 1.0) {
  }

  int packRgb() {
    return (Color4::redInt() << 24) | (Color4::greenInt() << 16) | (Color4::blueInt() << 8) | 255;
  }
};

#endif
