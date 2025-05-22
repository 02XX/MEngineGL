#pragma once

#include "Component/Component.hpp"
#include "UUID.hpp"
#include <memory>

#include "Entity/Material.hpp"

namespace MEngine
{
struct MaterialComponent : public Component
{
    MaterialType type;
    UUID materialID;
};
} // namespace MEngine
