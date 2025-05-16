#pragma once
#include "IEntity.hpp"

#include <memory>
namespace MEngine
{
class Entity : virtual public IEntity
{
  private:
    UUID mID = UUID();
    std::filesystem::path mPath = std::filesystem::path();

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
    inline virtual void SetID(const UUID &id) override
    {
        mID = id;
    }
    inline virtual void SetPath(const std::filesystem::path &path) override
    {
        mPath = path;
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
    }

    static void from_json(const json &j, MEngine::Entity &entity)
    {
        entity.SetID(MEngine::UUID(j.at("id").get<std::string>()));
        entity.SetPath(j.at("path").get<std::string>());
    }
};
} // namespace nlohmann
