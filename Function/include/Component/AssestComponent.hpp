#pragma once
#include "Component/Component.hpp"
#include <filesystem>
namespace MEngine
{
struct AssetsComponent : public Component
{
    std::filesystem::path path{};
    std::string name{};
};
} // namespace MEngine

// template <> struct entt::storage_type<MEngine::AssetsComponent>
// {
//     using type = entt::storage<MEngine::AssetsComponent>;
// };