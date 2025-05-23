//
// Created by 02 on 25-5-23.
//

#pragma once

#include "AssetImporter.hpp"
namespace MEngine
{
namespace Editor
{

class ShaderImporter : public AssetImporter
{
};

} // namespace Editor
} // namespace MEngine
namespace nlohmann
{
template <> struct adl_serializer<MEngine::Editor::ShaderImporter>
{
    static void to_json(json &j, const MEngine::Editor::ShaderImporter &importer)
    {
        j = static_cast<MEngine::Editor::AssetImporter>(importer);
    }
    static void from_json(const json &j, MEngine::Editor::ShaderImporter &importer)
    {
        static_cast<MEngine::Editor::AssetImporter &>(importer) = j;
    }
};
} // namespace nlohmann