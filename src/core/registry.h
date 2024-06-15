#ifndef TK_REGISTRY_H
#define TK_REGISTRY_H

#include "entt/entt.hpp"

class tkRegistry
{
	entt::registry EnttRegistry;

public:
	static entt::registry& Get();
};

#endif//TK_REGISTRY_H
