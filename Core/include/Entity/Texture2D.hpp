#pragma once
#include "Entity/Entity.hpp"
#include "Entity/ITexture.hpp"
#include "ImageUtil.hpp"
#include "Logger.hpp"
#include <glad/glad.h>
#include <magic_enum/magic_enum.hpp>
#include <refl.hpp>
namespace MEngine
{

class Texture2D final : public ITexture, public Entity
{
    friend struct nlohmann::adl_serializer<MEngine::Texture2D>;

  public:
    std::filesystem::path mImagePath;
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
    uint32_t mChannels = 0;
    GLsizei mMipLevels = 1;
    TextureFormatType mInternalFormat = TextureFormatType::RGBA8;
    int test = 0;
    GLuint mSamplerID = 0;

    Importer mImporter;
    GLuint mTextureID = 0;

  public:
    Texture2D();
    ~Texture2D();
    void SetImagePath(const std::filesystem::path &path) override;
    inline const std::filesystem::path &GetImagePath() const override
    {
        return mImagePath;
    }
    inline const uint32_t &GetWidth() const override
    {
        return mWidth;
    }
    inline const uint32_t &GetHeight() const override
    {
        return mHeight;
    }
    inline void SetImporter(const Importer &importer) override
    {
        mImporter.minFilter = importer.minFilter;
        mImporter.magFilter = importer.magFilter;
        mImporter.wrapS = importer.wrapS;
        mImporter.wrapT = importer.wrapT;
        mImporter.wrapR = importer.wrapR;
        mImporter.compareMode = importer.compareMode;
        mImporter.compareFunc = importer.compareFunc;
        mImporter.lodMin = importer.lodMin;
        mImporter.lodMax = importer.lodMax;
        mImporter.lodBias = importer.lodBias;
        mImporter.maxAnisotropy = importer.maxAnisotropy;
        mImporter.borderColor[0] = importer.borderColor[0];
        mImporter.borderColor[1] = importer.borderColor[1];
        mImporter.borderColor[2] = importer.borderColor[2];
        mImporter.borderColor[3] = importer.borderColor[3];
    }
    inline bool IsAnisotropicFilteringSupported() const
    {
        if (!glGetStringi || !glGetIntegerv)
        {
            return false;
        }
        GLint numExtensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

        for (GLint i = 0; i < numExtensions; ++i)
        {
            const char *extension = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i));
            if (extension && strcmp(extension, "GL_ARB_texture_filter_anisotropic") == 0)
            {
                return true;
            }
        }
        GLfloat maxAniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        return maxAniso > 1.0f;
    }
    inline GLuint GetTextureID() const override
    {
        return mTextureID;
    }
    inline GLuint GetSamplerID() const override
    {
        return mSamplerID;
    }
    inline const Importer &GetImporter() const override
    {
        return mImporter;
    }
    inline const uint32_t &GetChannels() const override
    {
        return mChannels;
    }
    void Update();
};

} // namespace MEngine
namespace magic_enum
{
namespace customize
{
template <> struct enum_range<MEngine::FilterType>
{
    static constexpr int min = static_cast<int>(MEngine::FilterType::Nearest);
    static constexpr int max = static_cast<int>(MEngine::FilterType::LinearMipmapLinear);
    static_assert(max > min, "enum_range requires max > min");
};
template <> struct enum_range<MEngine::WrapModeType>
{
    static constexpr int min = static_cast<int>(MEngine::WrapModeType::Repeat);
    static constexpr int max = static_cast<int>(MEngine::WrapModeType::ClampToBorder);
    static_assert(max > min, "enum_range requires max > min");
};
template <> struct enum_range<MEngine::CompareModeType>
{
    static constexpr int min = static_cast<int>(MEngine::CompareModeType::None);
    static constexpr int max = static_cast<int>(MEngine::CompareModeType::CompareRefToTexture);
    static_assert(max > min, "enum_range requires max > min");
};
template <> struct enum_range<MEngine::CompareFuncType>
{
    static constexpr int min = static_cast<int>(MEngine::CompareFuncType::Never);
    static constexpr int max = static_cast<int>(MEngine::CompareFuncType::Always);
    static_assert(max > min, "enum_range requires max > min");
};
template <> struct enum_range<MEngine::TextureFormatType>
{
    static constexpr int min = static_cast<int>(MEngine::TextureFormatType::RGB);
    static constexpr int max = static_cast<int>(MEngine::TextureFormatType::DepthStencil);
    static_assert(max > min, "enum_range requires max > min");
};
} // namespace customize
} // namespace magic_enum
namespace nlohmann
{
template <> struct adl_serializer<MEngine::Texture2D>
{
    static void to_json(json &j, const MEngine::Texture2D &texture)
    {
        j = static_cast<MEngine::Entity>(texture);
        j["image_path"] = texture.mImagePath;
        j["width"] = texture.mWidth;
        j["height"] = texture.mHeight;
        j["test"] = texture.test;
        j["importer"]["mip_levels"] = texture.mImporter.mipLevels;
        j["importer"]["min_filter"] = magic_enum::enum_name(texture.mImporter.minFilter);
        j["importer"]["mag_filter"] = magic_enum::enum_name(texture.mImporter.magFilter);
        j["importer"]["wrap_s"] = magic_enum::enum_name(texture.mImporter.wrapS);
        j["importer"]["wrap_t"] = magic_enum::enum_name(texture.mImporter.wrapT);
        j["importer"]["wrap_r"] = magic_enum::enum_name(texture.mImporter.wrapR);
        j["importer"]["compare_mode"] = magic_enum::enum_name(texture.mImporter.compareMode);
        j["importer"]["compare_func"] = magic_enum::enum_name(texture.mImporter.compareFunc);
        j["importer"]["lod_min"] = texture.mImporter.lodMin;
        j["importer"]["lod_max"] = texture.mImporter.lodMax;
        j["importer"]["lod_bias"] = texture.mImporter.lodBias;
        j["importer"]["max_anisotropy"] = texture.mImporter.maxAnisotropy;
        j["importer"]["border_color"] = {texture.mImporter.borderColor[0], texture.mImporter.borderColor[1],
                                         texture.mImporter.borderColor[2], texture.mImporter.borderColor[3]};
    }
    static void from_json(const json &j, MEngine::Texture2D &texture)
    {
        static_cast<MEngine::Entity &>(texture) = j;
        texture.mImagePath = j.at("image_path").get<std::string>();
        texture.mWidth = j.at("width").get<uint32_t>();
        texture.mHeight = j.at("height").get<uint32_t>();
        texture.mImporter.mipLevels = j.at("importer").at("mip_levels").get<GLsizei>();
        std::string minFilter = j.at("importer").at("min_filter").get<std::string>();
        auto enumMinFilter = magic_enum::enum_cast<MEngine::FilterType>(minFilter);
        texture.mImporter.minFilter = enumMinFilter.has_value() ? enumMinFilter.value() : MEngine::FilterType::Linear;
        std::string magFilter = j.at("importer").at("mag_filter").get<std::string>();
        auto enumMagFilter = magic_enum::enum_cast<MEngine::FilterType>(magFilter);
        texture.mImporter.magFilter = enumMagFilter.has_value() ? enumMagFilter.value() : MEngine::FilterType::Linear;
        std::string wrapS = j.at("importer").at("wrap_s").get<std::string>();
        auto enumWrapS = magic_enum::enum_cast<MEngine::WrapModeType>(wrapS);
        texture.mImporter.wrapS = enumWrapS.has_value() ? enumWrapS.value() : MEngine::WrapModeType::Repeat;
        std::string wrapT = j.at("importer").at("wrap_t").get<std::string>();
        auto enumWrapT = magic_enum::enum_cast<MEngine::WrapModeType>(wrapT);
        texture.mImporter.wrapT = enumWrapT.has_value() ? enumWrapT.value() : MEngine::WrapModeType::Repeat;
        std::string wrapR = j.at("importer").at("wrap_r").get<std::string>();
        auto enumWrapR = magic_enum::enum_cast<MEngine::WrapModeType>(wrapR);
        texture.mImporter.wrapR = enumWrapR.has_value() ? enumWrapR.value() : MEngine::WrapModeType::Repeat;
        std::string compareMode = j.at("importer").at("compare_mode").get<std::string>();
        auto enumCompareMode = magic_enum::enum_cast<MEngine::CompareModeType>(compareMode);
        texture.mImporter.compareMode =
            enumCompareMode.has_value() ? enumCompareMode.value() : MEngine::CompareModeType::None;
        std::string compareFunc = j.at("importer").at("compare_func").get<std::string>();
        auto enumCompareFunc = magic_enum::enum_cast<MEngine::CompareFuncType>(compareFunc);
        texture.mImporter.compareFunc =
            enumCompareFunc.has_value() ? enumCompareFunc.value() : MEngine::CompareFuncType::Lequal;
        texture.mImporter.lodMin = j.at("importer").at("lod_min").get<float>();
        texture.mImporter.lodMax = j.at("importer").at("lod_max").get<float>();
        texture.mImporter.lodBias = j.at("importer").at("lod_bias").get<float>();
        texture.mImporter.maxAnisotropy = j.at("importer").at("max_anisotropy").get<float>();
        auto borderColor = j.at("importer").at("border_color").get<std::vector<float>>();
        texture.mImporter.borderColor[0] = borderColor[0];
        texture.mImporter.borderColor[1] = borderColor[1];
        texture.mImporter.borderColor[2] = borderColor[2];
        texture.mImporter.borderColor[3] = borderColor[3];
        texture.test = j.at("test").get<int>();
    }
};
} // namespace nlohmann

REFL_AUTO(type(MEngine::Texture2D, bases<MEngine::ITexture>), field(mImagePath), field(mWidth), field(mHeight),
          field(mChannels), field(mMipLevels), field(mInternalFormat), field(mSamplerID), field(mImporter),
          field(mTextureID), field(test, Editable()))
