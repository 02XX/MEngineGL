#pragma once
#include "IEntity.hpp"
#include "UUID.hpp"

#include <memory>
namespace MEngine
{
class Entity : virtual public IEntity
{
  protected:
    UUID mID = UUIDGenerator()();
    std::filesystem::path mPath = std::filesystem::path();
    std::string mName = "Unknown";

  public:
    Entity() = default;
    virtual ~Entity() = default;
    inline virtual const UUID &GetID() const override
    {
        return mID;
    }
    inline virtual const std::filesystem::path &GetPath() const override
    {
        return mPath;
    }
    inline virtual const std::string &GetName() const override
    {
        return mName;
    }
    inline virtual void SetID(const UUID &id) override
    {
        mID = id;
    }
    inline virtual void SetPath(const std::filesystem::path &path) override
    {
        mPath = path;
    }
    inline virtual void SetName(const std::string &name) override
    {
        mName = name;
    }
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Entity>
{
    static void to_json(json &j, const MEngine::Entity &entity)
    {
        j["id"] = entity.GetID().ToString();
        j["path"] = entity.GetPath().string();
        j["name"] = entity.GetName();
    }

    static void from_json(const json &j, MEngine::Entity &entity)
    {
        entity.SetID(MEngine::UUID(j.at("id").get<std::string>()));
        entity.SetPath(j.at("path").get<std::string>());
        entity.SetName(j.at("name").get<std::string>());
    }
};
} // namespace nlohmann
