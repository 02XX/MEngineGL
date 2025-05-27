#pragma once
#include "Component/Component.hpp"
#include "UUID.hpp"
using namespace MEngine::Core;
namespace MEngine
{
namespace Function
{
struct TextureComponent : public Component
{
    UUID textureID;
};
} // namespace Function
} // namespace MEngine