#pragma once

#include "Component/Component.hpp"
#include <memory>

namespace MEngine
{
struct MeshComponent : public Component
{
    UUID meshID;
};
} // namespace MEngine