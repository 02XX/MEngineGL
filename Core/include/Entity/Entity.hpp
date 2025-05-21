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
class Entity
{
  public:
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
    }

    static void from_json(const json &j, MEngine::Entity &entity)
    {
        entity.ID = MEngine::UUID(j["id"].get<std::string>());
        entity.SourcePath = j["path"].get<std::string>();
        entity.Name = j["name"].get<std::string>();
    }
};
} // namespace nlohmann