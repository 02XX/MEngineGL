
#pragma once
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <vector>

namespace MEngine
{
namespace Editor
{
class AssetDatabase;

class AssetImporter
{
  protected:
    std::vector<std::string> supportedExtensions{};

  public:
    std::string name;
    std::filesystem::path assetPath;


  public:
    AssetImporter() = default;
    virtual ~AssetImporter() = default;
    void SaveAndReimport() const
    {
    }
};
} // namespace Editor
} // namespace MEngine
namespace nlohmann
{
template <> struct adl_serializer<MEngine::Editor::AssetImporter>
{
    static void to_json(json &j, const MEngine::Editor::AssetImporter &importer)
    {
    }
    static void from_json(const json &j, MEngine::Editor::AssetImporter &importer)
    {
    }
};
} // namespace nlohmann