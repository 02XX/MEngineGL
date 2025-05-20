#pragma once
#include "Entity/Entity.hpp"
#include "ImageUtil.hpp"
#include "Logger.hpp"
#include <glad/glad.h>
#include <magic_enum/magic_enum.hpp>
#include <refl.hpp>
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
    RGBA8 = GL_RGBA8,
    RGBA16 = GL_RGBA16,
    DepthComponent = GL_DEPTH_COMPONENT,
    DepthStencil = GL_DEPTH_STENCIL
};
struct Importer
{
    GLsizei mipLevels = 1;
    FilterType minFilter = FilterType::Linear;
    FilterType magFilter = FilterType::Linear;
    WrapModeType wrapS = WrapModeType::Repeat;
    WrapModeType wrapT = WrapModeType::Repeat;
    WrapModeType wrapR = WrapModeType::Repeat;
    CompareModeType compareMode = CompareModeType::None;
    CompareFuncType compareFunc = CompareFuncType::Lequal;
    float lodMin = -1000.0f;
    float lodMax = 1000.0f;
    float lodBias = 0.0f;
    float maxAnisotropy = 1.0f;
    float borderColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
};
class Texture2D final : public Entity
{
  public:
    Path ImagePath;
    Int Width = 0;
    Int Height = 0;
    Int Channels = 0;
    Property<Importer> Importer;

    GLsizei mMipLevels = 1;
    TextureFormatType mInternalFormat = TextureFormatType::RGBA8;
    int test = 0;
    GLuint mSamplerID = 0;
    GLuint mTextureID = 0;

  public:
    Texture2D();
    ~Texture2D();
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
    void Update() override;
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
        j["image_path"] = texture.ImagePath.Get().string();
        j["width"] = texture.Width.Get();
        j["height"] = texture.Height.Get();
        j["test"] = texture.test;
        j["importer"]["mip_levels"] = texture.Importer.Get().mipLevels;
        j["importer"]["min_filter"] = magic_enum::enum_name(texture.Importer.Get().minFilter);
        j["importer"]["mag_filter"] = magic_enum::enum_name(texture.Importer.Get().magFilter);
        j["importer"]["wrap_s"] = magic_enum::enum_name(texture.Importer.Get().wrapS);
        j["importer"]["wrap_t"] = magic_enum::enum_name(texture.Importer.Get().wrapT);
        j["importer"]["wrap_r"] = magic_enum::enum_name(texture.Importer.Get().wrapR);
        j["importer"]["compare_mode"] = magic_enum::enum_name(texture.Importer.Get().compareMode);
        j["importer"]["compare_func"] = magic_enum::enum_name(texture.Importer.Get().compareFunc);
        j["importer"]["lod_min"] = texture.Importer.Get().lodMin;
        j["importer"]["lod_max"] = texture.Importer.Get().lodMax;
        j["importer"]["lod_bias"] = texture.Importer.Get().lodBias;
        j["importer"]["max_anisotropy"] = texture.Importer.Get().maxAnisotropy;
        j["importer"]["border_color"] = {texture.Importer.Get().borderColor[0], texture.Importer.Get().borderColor[1],
                                         texture.Importer.Get().borderColor[2], texture.Importer.Get().borderColor[3]};
    }
    static void from_json(const json &j, MEngine::Texture2D &texture)
    {
        static_cast<MEngine::Entity &>(texture) = j;
        texture.ImagePath = j.at("image_path").get<std::string>();
        texture.Width = j.at("width").get<uint32_t>();
        texture.Height = j.at("height").get<uint32_t>();
        MEngine::Importer importer;
        importer.mipLevels = j.at("importer").at("mip_levels").get<GLsizei>();
        std::string minFilter = j.at("importer").at("min_filter").get<std::string>();
        auto minFilterEnum = magic_enum::enum_cast<MEngine::FilterType>(minFilter);
        if (minFilterEnum.has_value())
        {
            importer.minFilter = minFilterEnum.value();
        }
        else
        {
            throw std::runtime_error("Invalid min_filter: " + minFilter);
        }
        std::string magFilter = j.at("importer").at("mag_filter").get<std::string>();
        auto magFilterEnum = magic_enum::enum_cast<MEngine::FilterType>(magFilter);
        if (magFilterEnum.has_value())
        {
            importer.magFilter = magFilterEnum.value();
        }
        else
        {
            throw std::runtime_error("Invalid mag_filter: " + magFilter);
        }
        std::string wrapS = j.at("importer").at("wrap_s").get<std::string>();
        auto wrapSEnum = magic_enum::enum_cast<MEngine::WrapModeType>(wrapS);
        if (wrapSEnum.has_value())
        {
            importer.wrapS = wrapSEnum.value();
        }
        else
        {
            throw std::runtime_error("Invalid wrap_s: " + wrapS);
        }
        std::string wrapT = j.at("importer").at("wrap_t").get<std::string>();
        auto wrapTEnum = magic_enum::enum_cast<MEngine::WrapModeType>(wrapT);
        if (wrapTEnum.has_value())
        {
            importer.wrapT = wrapTEnum.value();
        }
        else
        {
            throw std::runtime_error("Invalid wrap_t: " + wrapT);
        }
        std::string wrapR = j.at("importer").at("wrap_r").get<std::string>();
        auto wrapREnum = magic_enum::enum_cast<MEngine::WrapModeType>(wrapR);
        if (wrapREnum.has_value())
        {
            importer.wrapR = wrapREnum.value();
        }
        else
        {
            throw std::runtime_error("Invalid wrap_r: " + wrapR);
        }
        std::string compareMode = j.at("importer").at("compare_mode").get<std::string>();
        auto compareModeEnum = magic_enum::enum_cast<MEngine::CompareModeType>(compareMode);
        if (compareModeEnum.has_value())
        {
            importer.compareMode = compareModeEnum.value();
        }
        else
        {
            throw std::runtime_error("Invalid compare_mode: " + compareMode);
        }
        std::string compareFunc = j.at("importer").at("compare_func").get<std::string>();
        auto compareFuncEnum = magic_enum::enum_cast<MEngine::CompareFuncType>(compareFunc);
        if (compareFuncEnum.has_value())
        {
            importer.compareFunc = compareFuncEnum.value();
        }
        else
        {
            throw std::runtime_error("Invalid compare_func: " + compareFunc);
        }
        importer.lodMin = j.at("importer").at("lod_min").get<float>();
        importer.lodMax = j.at("importer").at("lod_max").get<float>();
        importer.lodBias = j.at("importer").at("lod_bias").get<float>();
        importer.maxAnisotropy = j.at("importer").at("max_anisotropy").get<float>();
        auto borderColor = j.at("importer").at("border_color").get<std::vector<float>>();
        if (borderColor.size() == 4)
        {
            importer.borderColor[0] = borderColor[0];
            importer.borderColor[1] = borderColor[1];
            importer.borderColor[2] = borderColor[2];
            importer.borderColor[3] = borderColor[3];
        }
        else
        {
            throw std::runtime_error("Invalid border_color format");
        }
        texture.Importer = importer;
        texture.test = j.at("test").get<int>();
    }
};
} // namespace nlohmann
