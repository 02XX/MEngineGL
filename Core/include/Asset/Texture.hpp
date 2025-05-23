#pragma once
#include "Asset/Asset.hpp"
#include "ImageUtil.hpp"
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
    RGBA8 = GL_RGBA8,
    RGBA16 = GL_RGBA16,
    DepthComponent = GL_DEPTH_COMPONENT,
    DepthStencil = GL_DEPTH_STENCIL
};

class Texture : public Asset
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

    int Width = 0;
    int Height = 0;
    int Channels = 0;

  public:
    GLuint mSamplerID = 0;
    GLuint mTextureID = 0;

  public:
    virtual ~Texture() = default;
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
template <> struct adl_serializer<MEngine::Texture>
{
    static void to_json(json &j, const MEngine::Texture &texture)
    {
        j = static_cast<MEngine::Asset>(texture);
        j["width"] = texture.Width;
        j["height"] = texture.Height;
    }
    static void from_json(const json &j, MEngine::Texture &texture)
    {
        static_cast<MEngine::Asset &>(texture) = j;
    }
};
} // namespace nlohmann
