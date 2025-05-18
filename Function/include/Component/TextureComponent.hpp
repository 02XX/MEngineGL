#pragma once
#include "Component/Component.hpp"
#include "Entity/ITexture.hpp"
#include <entt/entt.hpp>
#include <filesystem>
#include <memory>

namespace MEngine
{

struct TextureComponent : public Component
{
    std::shared_ptr<ITexture> texture;
};
} // namespace MEngine
REFL_AUTO(type(MEngine::TextureComponent, bases<MEngine::Component>), field(texture))
