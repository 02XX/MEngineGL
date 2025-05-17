#pragma once
#include "Entity/Entity.hpp"
#include "Entity/ITexture.hpp"
#include "ImageUtil.hpp"
#include "Logger.hpp"
#include <glad/glad.h>
#include <magic_enum/magic_enum.hpp>
namespace MEngine
{
enum class FilterType : GLenum
{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};
enum class WrapModeType : GLenum
{
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER
};
enum class CompareModeType : GLenum
{
    None = GL_NONE,
    CompareRefToTexture = GL_COMPARE_REF_TO_TEXTURE
};
enum class CompareFuncType : GLenum
{
    Never = GL_NEVER,
    Less = GL_LESS,
    Equal = GL_EQUAL,
    Lequal = GL_LEQUAL,
    Greater = GL_GREATER,
    Notequal = GL_NOTEQUAL,
    Gequal = GL_GEQUAL,
    Always = GL_ALWAYS
};
enum class TextureFormatType : GLenum
{
    RGB = GL_RGB,
    RGBA = GL_RGBA,
    DepthComponent = GL_DEPTH_COMPONENT,
    DepthStencil = GL_DEPTH_STENCIL
};
class Texture2D final : public ITexture, public Entity
{
    friend struct nlohmann::adl_serializer<MEngine::Texture2D>;

  private:
    std::filesystem::path mImagePath;
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
    uint32_t mChannels = 0;
    GLsizei mMipLevels = 1;
    TextureFormatType mInternalFormat = TextureFormatType::RGBA;

    GLuint mSamplerID = 0;
    FilterType mMinFilter = FilterType::Linear;
    FilterType mMagFilter = FilterType::Linear;
    WrapModeType mWrapS = WrapModeType::Repeat;
    WrapModeType mWrapT = WrapModeType::Repeat;
    WrapModeType mWrapR = WrapModeType::Repeat;
    CompareModeType mCompareMode = CompareModeType::None;
    CompareFuncType mCompareFunc = CompareFuncType::Lequal;
    float mLodMin = -1000.0f;
    float mLodMax = 1000.0f;
    float mLodBias = 0.0f;
    float mMaxAnisotropy = 1.0f;
    float mBorderColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
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
    void SetSampler();
    inline bool IsAnisotropicFilteringSupported() const
    {
        if (!glGetString)
            return false;
        const char *extensions = (const char *)glGetString(GL_EXTENSIONS);
        return extensions && strstr(extensions, "GL_EXT_texture_filter_anisotropic");
    }
    inline GLuint GetTextureID() const override
    {
        return mTextureID;
    }
    inline GLuint GetSamplerID() const override
    {
        return mSamplerID;
    }
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
        j["importer"]["mip_levels"] = texture.mMipLevels;
        j["importer"]["min_filter"] = magic_enum::enum_name(texture.mMinFilter);
        j["importer"]["mag_filter"] = magic_enum::enum_name(texture.mMagFilter);
        j["importer"]["wrap_s"] = magic_enum::enum_name(texture.mWrapS);
        j["importer"]["wrap_t"] = magic_enum::enum_name(texture.mWrapT);
        j["importer"]["wrap_r"] = magic_enum::enum_name(texture.mWrapR);
        j["importer"]["compare_mode"] = magic_enum::enum_name(texture.mCompareMode);
        j["importer"]["compare_func"] = magic_enum::enum_name(texture.mCompareFunc);
        j["importer"]["lod_min"] = texture.mLodMin;
        j["importer"]["lod_max"] = texture.mLodMax;
        j["importer"]["lod_bias"] = texture.mLodBias;
        j["importer"]["max_anisotropy"] = texture.mMaxAnisotropy;
        j["importer"]["border_color"] = {texture.mBorderColor[0], texture.mBorderColor[1], texture.mBorderColor[2],
                                         texture.mBorderColor[3]};
    }
    static void from_json(const json &j, MEngine::Texture2D &texture)
    {
        static_cast<MEngine::Entity &>(texture) = j;
        texture.mImagePath = j.at("image_path").get<std::string>();
        texture.mWidth = j.at("width").get<uint32_t>();
        texture.mHeight = j.at("height").get<uint32_t>();
        texture.mMipLevels = j.at("importer").at("mip_levels").get<GLsizei>();
        std::string minFilter = j.at("importer").at("min_filter").get<std::string>();
        auto enumMinFilter = magic_enum::enum_cast<MEngine::FilterType>(minFilter);
        texture.mMinFilter = enumMinFilter.has_value() ? enumMinFilter.value() : MEngine::FilterType::Linear;
        std::string magFilter = j.at("importer").at("mag_filter").get<std::string>();
        auto enumMagFilter = magic_enum::enum_cast<MEngine::FilterType>(magFilter);
        texture.mMagFilter = enumMagFilter.has_value() ? enumMagFilter.value() : MEngine::FilterType::Linear;
        std::string wrapS = j.at("importer").at("wrap_s").get<std::string>();
        auto enumWrapS = magic_enum::enum_cast<MEngine::WrapModeType>(wrapS);
        texture.mWrapS = enumWrapS.has_value() ? enumWrapS.value() : MEngine::WrapModeType::Repeat;
        std::string wrapT = j.at("importer").at("wrap_t").get<std::string>();
        auto enumWrapT = magic_enum::enum_cast<MEngine::WrapModeType>(wrapT);
        texture.mWrapT = enumWrapT.has_value() ? enumWrapT.value() : MEngine::WrapModeType::Repeat;
        std::string wrapR = j.at("importer").at("wrap_r").get<std::string>();
        auto enumWrapR = magic_enum::enum_cast<MEngine::WrapModeType>(wrapR);
        texture.mWrapR = enumWrapR.has_value() ? enumWrapR.value() : MEngine::WrapModeType::Repeat;
        std::string compareMode = j.at("importer").at("compare_mode").get<std::string>();
        auto enumCompareMode = magic_enum::enum_cast<MEngine::CompareModeType>(compareMode);
        texture.mCompareMode = enumCompareMode.has_value() ? enumCompareMode.value() : MEngine::CompareModeType::None;
        std::string compareFunc = j.at("importer").at("compare_func").get<std::string>();
        auto enumCompareFunc = magic_enum::enum_cast<MEngine::CompareFuncType>(compareFunc);
        texture.mCompareFunc = enumCompareFunc.has_value() ? enumCompareFunc.value() : MEngine::CompareFuncType::Lequal;
        texture.mLodMin = j.at("importer").at("lod_min").get<float>();
        texture.mLodMax = j.at("importer").at("lod_max").get<float>();
        texture.mLodBias = j.at("importer").at("lod_bias").get<float>();
        texture.mMaxAnisotropy = j.at("importer").at("max_anisotropy").get<float>();
        auto borderColor = j.at("importer").at("border_color").get<std::vector<float>>();
        texture.mBorderColor[0] = borderColor[0];
        texture.mBorderColor[1] = borderColor[1];
        texture.mBorderColor[2] = borderColor[2];
        texture.mBorderColor[3] = borderColor[3];
    }
};
} // namespace nlohmann