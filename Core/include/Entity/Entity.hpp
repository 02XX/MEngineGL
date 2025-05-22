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
enum class EntityType
{
    None,
    Folder,
    File,
    Material,
    Mesh,
    PhongMaterial,
    Texture2D,
    TextureCube,
    Model,
    Animation,
    Shader,
    Audio,
    Pipeline
};
class Entity
{
  public:
    EntityType Type = EntityType::None;
    UUID ID = UUIDGenerator()();
    std::filesystem::path SourcePath = std::filesystem::path();
    std::string Name = "Unknown";

  public:
    Entity() = default;
    virtual ~Entity() = default;
    virtual void Update() {};
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Entity>
{
    static void to_json(json &j, const MEngine::Entity &entity)
    {
        auto uuid = entity.ID;
        j["id"] = uuid.ToString();
        j["path"] = entity.SourcePath.string();
        j["name"] = entity.Name;
        j["type"] = magic_enum::enum_name(entity.Type);
    }

    static void from_json(const json &j, MEngine::Entity &entity)
    {
        entity.ID = MEngine::UUID(j["id"].get<std::string>());
        entity.SourcePath = j["path"].get<std::string>();
        entity.Name = j["name"].get<std::string>();
        auto typeStr = j["type"].get<std::string>();
        auto type = magic_enum::enum_cast<MEngine::EntityType>(typeStr);
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