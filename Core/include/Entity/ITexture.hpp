#pragma once
#include "Entity/IEntity.hpp"
#include <glad/glad.h>
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
class ITexture : virtual public IEntity
{
  public:
    virtual ~ITexture() = default;
    virtual void SetImagePath(const std::filesystem::path &path) = 0;
    virtual const std::filesystem::path &GetImagePath() const = 0;
    virtual void SetImporter(const Importer &importer) = 0;

    virtual const uint32_t &GetWidth() const = 0;
    virtual const uint32_t &GetHeight() const = 0;
    virtual const uint32_t &GetChannels() const = 0;
    virtual const Importer &GetImporter() const = 0;

    virtual GLuint GetTextureID() const = 0;
    virtual GLuint GetSamplerID() const = 0;
};
} // namespace MEngine

REFL_AUTO(type(MEngine::ITexture, bases<MEngine::IEntity>))