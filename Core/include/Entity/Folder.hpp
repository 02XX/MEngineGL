#pragma once

#include "Entity/Entity.hpp"
#include <filesystem>

namespace MEngine
{
class Folder : public Entity
{
  public:
    std::filesystem::path FolderPath{};
    Folder()
    {
        Name = "Folder";
        Type = EntityType::Folder;
    }
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Folder>
{
    static void to_json(json &j, const MEngine::Folder &folder)
    {
        j = static_cast<MEngine::Entity>(folder);
        j["FolderPath"] = folder.FolderPath.string();
    }

    static void from_json(const json &j, MEngine::Folder &folder)
    {
        static_cast<MEngine::Entity &>(folder) = j;
        folder.FolderPath = j.at("FolderPath").get<std::string>();
    }
};
} // namespace nlohmann