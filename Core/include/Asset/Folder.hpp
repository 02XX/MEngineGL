#pragma once

#include "Asset/Asset.hpp"
#include <filesystem>

namespace MEngine
{
namespace Core
{
class Folder : public Asset
{
  public:
    Folder() = default;
};
} // namespace Core
} // namespace MEngine
namespace nlohmann
{
template <> struct adl_serializer<MEngine::Core::Folder>
{
    static void to_json(json &j, const MEngine::Core::Folder &folder)
    {
        j = static_cast<MEngine::Core::Asset>(folder);
        // j["FolderPath"] = folder.FolderPath.string();
    }

    static void from_json(const json &j, MEngine::Core::Folder &folder)
    {
        static_cast<MEngine::Core::Asset &>(folder) = j;
        // folder.FolderPath = j.at("FolderPath").get<std::string>();
    }
};
} // namespace nlohmann