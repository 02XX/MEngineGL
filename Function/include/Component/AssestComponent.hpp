#pragma once
#include "Component/Component.hpp"
#include <filesystem>
namespace MEngine
{
enum class AssetType
{
    None,
    Folder,
    File,
    PBRMaterial,
    PhongMaterial,
    Texture2D,
    TextureCube,
    Model,
    Animation,
    Shader,
    Audio
};
struct AssetsComponent : public Component
{
    std::filesystem::path path{};
    std::string name{};
    AssetType type = AssetType::None;
};
} // namespace MEngine

// template <> struct entt::storage_type<MEngine::AssetsComponent>
// {
//     using type = entt::storage<MEngine::AssetsComponent>;
// };