#include "System/MaterialSystem.hpp"
#include "Component/MaterialComponent.hpp"
#include <entt/entt.hpp>
namespace MEngine
{
namespace Function
{

void MaterialSystem::Init()
{
}

void MaterialSystem::Update(float deltaTime)
{
    auto view = mRegistry->view<MaterialComponent>();
    for (auto entity : view)
    {
        auto &materialComponent = view.get<MaterialComponent>(entity);
        auto materialHandle = materialComponent.materialHandle;
        // Initialize the material if it is not already initialized
    }
}

void MaterialSystem::Shutdown()
{
}

} // namespace Function
} // namespace MEngine