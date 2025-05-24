
#pragma once
#include "Asset/Texture.hpp"
#include "Importer/AssetImporter.hpp"
using namespace MEngine::Core;
namespace MEngine
{
namespace Editor
{

class TextureImporter final : public AssetImporter
{
  public:
    int MipmapLevels = 1;
    float MipmapBias = 0.0f;
    FilterType MinFilter = FilterType::Linear;
    FilterType MagFilter = FilterType::Linear;
    WrapModeType WrapU = WrapModeType::Repeat;
    WrapModeType WrapV = WrapModeType::Repeat;
    WrapModeType WrapW = WrapModeType::Repeat;
    CompareModeType compareMode = CompareModeType::None;
    CompareFuncType compareFunc = CompareFuncType::Lequal;
    float LodMin = -1000.0f;
    float LodMax = 1000.0f;
    float LodBias = 0.0f;
    int AnsioLevel = 0;
    float BorderColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

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
        j["MipmapLevels"] = importer.MipmapLevels;
        j["MipmapBias"] = importer.MipmapBias;
        j["MinFilter"] = magic_enum::enum_name(importer.MinFilter);
        j["MagFilter"] = magic_enum::enum_name(importer.MagFilter);
        j["WrapU"] = magic_enum::enum_name(importer.WrapU);
        j["WrapV"] = magic_enum::enum_name(importer.WrapV);
        j["WrapW"] = magic_enum::enum_name(importer.WrapW);
        j["compareMode"] = magic_enum::enum_name(importer.compareMode);
        j["compareFunc"] = magic_enum::enum_name(importer.compareFunc);
        j["LodMin"] = importer.LodMin;
        j["LodMax"] = importer.LodMax;
        j["LodBias"] = importer.LodBias;
        j["AnsioLevel"] = importer.AnsioLevel;
        j["BorderColor"] = {importer.BorderColor[0], importer.BorderColor[1], importer.BorderColor[2],
                            importer.BorderColor[3]};
    }
    static void from_json(const json &j, MEngine::Editor::TextureImporter &importer)
    {
        static_cast<MEngine::Editor::AssetImporter &>(importer) = j;
        importer.MipmapLevels = j.at("MipmapLevels").get<int>();
        importer.MipmapBias = j.at("MipmapBias").get<float>();
        auto minFilter = j.at("MinFilter").get<std::string>();
        auto filterEnumValue = magic_enum::enum_cast<MEngine::Core::FilterType>(minFilter);
        if (filterEnumValue.has_value())
        {
            importer.MinFilter = filterEnumValue.value();
        }
        else
        {
            throw std::runtime_error("Invalid MinFilter value");
        }
        auto magFilter = j.at("MagFilter").get<std::string>();
        auto magEnumValue = magic_enum::enum_cast<MEngine::Core::FilterType>(magFilter);
        if (magEnumValue.has_value())
        {
            importer.MagFilter = magEnumValue.value();
        }
        else
        {
            throw std::runtime_error("Invalid MagFilter value");
        }
        auto wrapU = j.at("WrapU").get<std::string>();
        auto wrapEnumValue = magic_enum::enum_cast<MEngine::Core::WrapModeType>(wrapU);
        if (wrapEnumValue.has_value())
        {
            importer.WrapU = wrapEnumValue.value();
        }
        else
        {
            throw std::runtime_error("Invalid WrapU value");
        }
        auto wrapV = j.at("WrapV").get<std::string>();
        wrapEnumValue = magic_enum::enum_cast<MEngine::Core::WrapModeType>(wrapV);
        if (wrapEnumValue.has_value())
        {
            importer.WrapV = wrapEnumValue.value();
        }
        else
        {
            throw std::runtime_error("Invalid WrapV value");
        }
        auto wrapW = j.at("WrapW").get<std::string>();
        wrapEnumValue = magic_enum::enum_cast<MEngine::Core::WrapModeType>(wrapW);
        if (wrapEnumValue.has_value())
        {
            importer.WrapW = wrapEnumValue.value();
        }
        else
        {
            throw std::runtime_error("Invalid WrapW value");
        }
        auto compareMode = j.at("compareMode").get<std::string>();
        auto compareEnumValue = magic_enum::enum_cast<MEngine::Core::CompareModeType>(compareMode);
        if (compareEnumValue.has_value())
        {
            importer.compareMode = compareEnumValue.value();
        }
        else
        {
            throw std::runtime_error("Invalid compareMode value");
        }
        auto compareFunc = j.at("compareFunc").get<std::string>();
        auto funcEnumValue = magic_enum::enum_cast<MEngine::Core::CompareFuncType>(compareFunc);
        if (funcEnumValue.has_value())
        {
            importer.compareFunc = funcEnumValue.value();
        }
        else
        {
            throw std::runtime_error("Invalid compareFunc value");
        }
        importer.LodMin = j.at("LodMin").get<float>();
        importer.LodMax = j.at("LodMax").get<float>();
        importer.LodBias = j.at("LodBias").get<float>();
        importer.AnsioLevel = j.at("AnsioLevel").get<int>();
        auto borderColor = j.at("BorderColor").get<std::vector<float>>();
        if (borderColor.size() == 4)
        {
            importer.BorderColor[0] = borderColor[0];
            importer.BorderColor[1] = borderColor[1];
            importer.BorderColor[2] = borderColor[2];
            importer.BorderColor[3] = borderColor[3];
        }
        else
        {
            throw std::runtime_error("Invalid BorderColor value");
        }
    }
};
} // namespace nlohmann
