#pragma once
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;
namespace MEngine
{
namespace Core
{
enum class AssetType
{
    None,
    Folder,
    Texture,
    Material,
    Mesh,
    Model,
    Animation,
    Audio,
    Shader,
    Prefab,
    Script
};
class Asset
{
  private:
    unsigned int mRefCount = 0;

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
} // namespace Core
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Core::Asset>
{
    static void to_json(json &j, const MEngine::Core::Asset &entity)
    {
        j["dirty"] = entity.isDirty;
    }

    static void from_json(const json &j, MEngine::Core::Asset &entity)
    {
        entity.isDirty = j.at("dirty").get<bool>();
    }
};
} // namespace nlohmann