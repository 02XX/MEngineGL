#pragma once

#include "Asset/Material.hpp"
#include "Component/Component.hpp"
#include "UUID.hpp"
#include <memory>

namespace MEngine
{
struct MaterialComponent : public Component
{
    PipelineType type;
    UUID materialID;
};
} // namespace MEngine
