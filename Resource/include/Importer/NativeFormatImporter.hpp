//
// Created by 02 on 25-5-23.
//

#pragma once

#include "AssetImporter.hpp"
namespace MEngine
{
namespace Editor
{

class NativeFormatImporter : public AssetImporter
{
};

} // namespace Editor
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Editor::NativeFormatImporter>
{
    static void to_json(json &j, const MEngine::Editor::NativeFormatImporter &importer)
    {
        j = static_cast<MEngine::Editor::AssetImporter>(importer);
    }
    static void from_json(const json &j, MEngine::Editor::NativeFormatImporter &importer)
    {
        static_cast<MEngine::Editor::AssetImporter &>(importer) = j;
    }
};
} // namespace nlohmann
