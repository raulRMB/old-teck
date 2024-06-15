#ifndef TC_SHAPE2D_H
#define TC_SHAPE2D_H

#include "../core/component.h"
#include "../core/def.h"

enum class eShape2DType : u8
{
  Rect = 0,
  Circle,
  Triangle,
};

struct tcShape2D : tkComponent
{
  eShape2DType Type = eShape2DType::Rect;
  tcShape2D(eShape2DType type) { Type = type; }
};

struct tcRect : tcShape2D
{
  v2 Dimensions = v2(1.f);
  tcRect() : tcShape2D(eShape2DType::Rect) {}
};

#endif //TC_SHAPE2D_H
