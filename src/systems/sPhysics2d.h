#include "../components/physics2d.h"
#include "../core/system.h"
#include "../components/transform2d.h"

class tsPhysics2d : public tkUpdateSystem
{
public:
    tsPhysics2d() = default;
    ~tsPhysics2d() = default;

    inline void Update() override
    {
        entt::registry& registry = tkRegistry::Get();
        auto view = registry.view<tcPhysics2d, tcTransform2d>();

        for (auto entity : view)
        {
            auto& physics = view.get<tcPhysics2d>(entity);
            auto& transform = view.get<tcTransform2d>(entity);

            transform.Position += physics.Velocity;
        }
    }
};