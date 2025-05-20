#pragma once
#include "UUID.hpp"
#include <glad/glad.h>
#include <memory>
namespace MEngine
{
class Entity
{
  public:
    UID ID = UUIDGenerator()();
    Path SourcePath = std::filesystem::path();
    String Name = "Unknown";

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
        auto uuid = entity.ID.Get();
        j["id"] = uuid.ToString();
        j["path"] = entity.SourcePath.Get().string();
        j["name"] = entity.Name.Get();
    }

    static void from_json(const json &j, MEngine::Entity &entity)
    {
        entity.ID = MEngine::UUID(j["id"].get<std::string>());
        entity.SourcePath = j["path"].get<std::string>();
        entity.Name = j["name"].get<std::string>();
    }
};
} // namespace nlohmann