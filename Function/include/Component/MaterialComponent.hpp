#pragma once

#include "Component/Component.hpp"
#include "Entity/IMaterial.hpp"
#include <memory>

namespace MEngine
{
struct MaterialComponent : public Component
{
    std::shared_ptr<IMaterial> material;
};
} // namespace MEngine
REFL_AUTO(type(MEngine::MaterialComponent, bases<MEngine::Component>), field(material))