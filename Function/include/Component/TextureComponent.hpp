#pragma once
#include "Component/Component.hpp"
#include "UUID.hpp"
#include <entt/entt.hpp>
#include <filesystem>
#include <memory>

namespace MEngine
{

struct TextureComponent : public Component
{
    UUID textureID;
};
} // namespace MEngine
