#pragma once

#include "Component/Component.hpp"
#include "Entity/IMesh.hpp"
#include <memory>

namespace MEngine
{
struct MeshComponent : public Component
{
    std::shared_ptr<IMesh> mesh;
};
} // namespace MEngine