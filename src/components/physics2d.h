#ifndef TK_PHYSICS2D_H
#define TK_PHYSICS2D_H

#include "../core/def.h"

struct tcPhysics2d
{
  v2 Velocity = v2(0.f);
  f32 AngularVelocity = 0.f;
};

#endif //TK_PHYSICS2D_H
