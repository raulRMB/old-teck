#ifndef TK_SYSTEM_H
#define TK_SYSTEM_H

#include "def.h"
#include <webgpu/webgpu_cpp.h>
#include "../core/registry.h"

class tkSystem
{
protected:
  friend class tkEngine;
  
  static entt::registry& GetRegistry();
  static entt::entity CreateEntity() { return tkRegistry::Get().create(); }
  template <typename C>
  static C& GetComponent(const entt::entity& entity) { return tkRegistry::Get().get<C>(entity); }
  
  template <typename C>
  static void AddComponent(const entt::entity& entity, C& Component) { tkRegistry::Get().emplace<C>(entity, Component); }

  virtual void Init() {};

  template <typename...C>
  static auto GetView() { return tkRegistry::Get().view<C...>(); }
};

class tkUpdateSystem : public tkSystem
{
protected:
  friend class tkEngine;
  virtual void Update() = 0;
};

class tkRenderSystem : public tkSystem
{
protected:
  friend class tkRenderer;
  virtual void Render(wgpu::Device&, wgpu::RenderPassEncoder&) = 0;
};

#endif//TK_SYSTEM_H
