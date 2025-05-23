#pragma once
#include "UUID.hpp"
#include <filesystem>
#include <glad/glad.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;
namespace MEngine
{
class Asset
{
  private:
  public:
    bool isDirty = false;

    std::string Name = "Asset";
    inline void MarkDirty()
    {
        isDirty = true;
    }
    Asset() = default;
    virtual ~Asset() = default;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Asset>
{
    static void to_json(json &j, const MEngine::Asset &entity)
    {
        auto uuid = entity.ID;
        j["id"] = uuid.ToString();
        j["path"] = entity.SourcePath.string();
        j["name"] = entity.Name;
        j["type"] = magic_enum::enum_name(entity.Type);
    }

    static void from_json(const json &j, MEngine::Asset &entity)
    {
        entity.ID = MEngine::UUID(j["id"].get<std::string>());
        entity.SourcePath = j["path"].get<std::string>();
        entity.Name = j["name"].get<std::string>();
        auto typeStr = j["type"].get<std::string>();
        auto type = magic_enum::enum_cast<MEngine::AssetType>(typeStr);
        if (type.has_value())
        {
            entity.Type = type.value();
        }
        else
        {
            throw std::runtime_error("Invalid entity type: " + typeStr);
        }
    }
};
} // namespace nlohmann