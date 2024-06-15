#ifndef TC_TRANSFORM2D_H
#define TC_TRANSFORM2D_H

#include "../core/component.h"
#include "../core/def.h"

struct tcTransform2d : tkComponent
{
  v2 Position = v2(0.f);
  f32 Angle = 0.f;
  f32 Scale = 1.f;
};

#endif //TC_TRANSFORM2D_H
