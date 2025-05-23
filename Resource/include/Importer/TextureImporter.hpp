
#pragma once
#include "Importer/AssetImporter.hpp"
namespace MEngine
{
namespace Editor
{

class TextureImporter final : public AssetImporter
{
  public:
    ~TextureImporter() override = default;
};
} // namespace Editor
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Editor::TextureImporter>
{
    static void to_json(json &j, const MEngine::Editor::TextureImporter &importer)
    {
        j = static_cast<MEngine::Editor::AssetImporter>(importer);
    }
    static void from_json(const json &j, MEngine::Editor::TextureImporter &importer)
    {
        static_cast<MEngine::Editor::AssetImporter &>(importer) = j;
    }
};
} // namespace nlohmann
