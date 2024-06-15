#include "system.h"
#include "registry.h"

entt::registry& tkSystem::GetRegistry()
{
  return tkRegistry::Get();
}


