#pragma once
#include "Asset/Material.hpp"
#include "Component/Component.hpp"
#include "Handle.hpp"

using namespace MEngine::Core;
namespace MEngine
{
namespace Function
{
struct MaterialComponent : public Component
{
    Handle<Material> materialHandle;
};
} // namespace Function
} // namespace MEngine
