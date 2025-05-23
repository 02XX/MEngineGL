
#pragma once
#include "Importer/AssetImporter.hpp"
namespace MEngine
{
namespace Editor
{

class FBXImporter final : public AssetImporter
{
  public:
    FBXImporter();
    ~FBXImporter() override = default;
};
} // namespace Editor
} // namespace MEngine
namespace nlohmann
{
template <> struct adl_serializer<MEngine::Editor::FBXImporter>
{
    static void to_json(json &j, const MEngine::Editor::FBXImporter &importer)
    {
        j = static_cast<MEngine::Editor::AssetImporter>(importer);
    }
    static void from_json(const json &j, MEngine::Editor::FBXImporter &importer)
    {
        static_cast<MEngine::Editor::AssetImporter &>(importer) = j;
    }
};
} // namespace nlohmann