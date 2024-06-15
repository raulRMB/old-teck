#include "scene.h"
#include "../core/registry.h"
#include "../components/transform2d.h"
#include "../components/shape2d.h"
#include "../components/physics2d.h"

void tkScene::BeginPlay()
{
  entt::registry& registry = tkRegistry::Get();
  for (i32 i = 0; i < 100; i++)
  {
    entt::entity entity = registry.create();
    tcTransform2d transform;
    transform.Position = v2(0.f);
    registry.emplace<tcTransform2d>(entity, transform);
    tcRect rect;
    rect.Dimensions = v2(.1f, .1f);
    registry.emplace<tcRect>(entity, rect);
    tcPhysics2d physics;
    physics.Velocity = v2((2.f * ((f32)rand() / (f32)RAND_MAX) - 1.0f) / 100.f, (2.f * ((f32)rand() / (f32)RAND_MAX) - 1.0f) / 100.f);
    registry.emplace<tcPhysics2d>(entity, physics);
  }
//  entt::entity entity = registry.create();
//  tcTransform2d transform;
//  transform.Position = v2(.1f, 0.2f);
//  registry.emplace<tcTransform2d>(entity, transform);
//  tcRect rect;
//  rect.Dimensions = v2(.1f, .1f);
//  registry.emplace<tcRect>(entity, rect);
//
//  entity = registry.create();
//  transform.Position = v2(.4f, 0.2f);
//  registry.emplace<tcTransform2d>(entity, transform);
//  rect.Dimensions = v2(.1f, .1f);
//  registry.emplace<tcRect>(entity, rect);
}

void tkScene::CleanUpScene()
{
}
