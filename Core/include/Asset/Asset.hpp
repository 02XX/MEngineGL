#pragma once
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
    unsigned int RefCount = 0;
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
        j["dirty"] = entity.isDirty;
    }

    static void from_json(const json &j, MEngine::Asset &entity)
    {
        entity.isDirty = j.at("dirty").get<bool>();
    }
};
} // namespace nlohmann