#include "registry.h"

entt::registry& tkRegistry::Get()
{
  static entt::registry instance;
  return instance;
}
