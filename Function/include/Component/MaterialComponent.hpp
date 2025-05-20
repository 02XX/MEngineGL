#pragma once

#include "Component/Component.hpp"
#include "Entity/IMaterial.hpp"
#include "UUID.hpp"
#include <memory>

namespace MEngine
{
struct MaterialComponent : public Component
{
    UUID materialID;
};
} // namespace MEngine
