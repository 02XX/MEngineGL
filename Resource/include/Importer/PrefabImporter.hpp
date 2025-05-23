//
// Created by 02 on 25-5-23.
//

#pragma once

#include "AssetImporter.hpp"
namespace MEngine
{
namespace Editor
{

class PrefabImporter : public AssetImporter
{
};

} // namespace Editor
} // namespace MEngine
namespace nlohmann
{
template <> struct adl_serializer<MEngine::Editor::PrefabImporter>
{
    static void to_json(json &j, const MEngine::Editor::PrefabImporter &importer)
    {
        j = static_cast<MEngine::Editor::AssetImporter>(importer);
    }
    static void from_json(const json &j, MEngine::Editor::PrefabImporter &importer)
    {
        static_cast<MEngine::Editor::AssetImporter &>(importer) = j;
    }
};
} // namespace nlohmann