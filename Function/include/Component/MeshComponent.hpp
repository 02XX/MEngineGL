#pragma once

#include "Component/Component.hpp"
#include <memory>

namespace MEngine
{
struct MeshComponent : public Component
{
    UUID modelID;
    int meshIndex;
};
} // namespace MEngine