#pragma once

#include "Asset/Asset.hpp"
#include <filesystem>

namespace MEngine
{
class Folder : public Asset
{
  public:
    std::filesystem::path FolderPath{};
    Folder() = default;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Folder>
{
    static void to_json(json &j, const MEngine::Folder &folder)
    {
        j = static_cast<MEngine::Asset>(folder);
        j["FolderPath"] = folder.FolderPath.string();
    }

    static void from_json(const json &j, MEngine::Folder &folder)
    {
        static_cast<MEngine::Asset &>(folder) = j;
        folder.FolderPath = j.at("FolderPath").get<std::string>();
    }
};
} // namespace nlohmann