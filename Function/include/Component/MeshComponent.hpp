#pragma once

#include "Component/Component.hpp"
#include <memory>

namespace MEngine
{
namespace Function
{
struct MeshComponent : public Component
{
    UUID modelID;
    int meshIndex;
};
} // namespace Function
} // namespace MEngine