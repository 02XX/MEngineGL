#include "Entity/Texture2D.hpp"

namespace MEngine
{
Texture2D::Texture2D()
{
    Name = "Texture2D";
    glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);
    glCreateSamplers(1, &mSamplerID);
}
Texture2D::~Texture2D()
{
    glDeleteTextures(1, &mTextureID);
    glDeleteSamplers(1, &mSamplerID);
}
void Texture2D::Update()
{
    if (std::filesystem::exists(ImagePath))
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(ImagePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        Width = width;
        Height = height;
        Channels = channels;
        if (data)
        {
            Importer.mipLevels =
                std::min(1 + static_cast<GLsizei>(std::floor(std::log2(std::max(width, height)))), Importer.mipLevels);
            glTextureStorage2D(mTextureID, Importer.mipLevels, (GLenum)mInternalFormat, width, height);
            glTextureSubImage2D(mTextureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            if (Importer.mipLevels > 1)
            {
                glGenerateTextureMipmap(mTextureID);
            }
            stbi_image_free(data);
        }
        else
        {
            LogError("Failed to load texture: {}", ImagePath.string());
        }
    }
    else // TODO: 默认纹理
    {
        LogWarn("Texture file does not exist: {}, 记得后续重构添加默认纹理", ImagePath.string());
        return;
    }
    // 设置缩小过滤器（包含mipmap）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, (GLint)Importer.minFilter);

    // 设置放大过滤器
    glSamplerParameteri(mSamplerID, GL_TEXTURE_MAG_FILTER, (GLint)Importer.magFilter);

    // 设置纹理环绕方式
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_S, (GLint)Importer.wrapR); // S轴（U）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_T, (GLint)Importer.wrapT); // T轴（V）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_R, (GLint)Importer.wrapR); // R轴（3D纹理需要）

    // 比较模式（用于阴影贴图）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_COMPARE_MODE, (GLint)Importer.compareMode);
    glSamplerParameteri(mSamplerID, GL_TEXTURE_COMPARE_FUNC, (GLint)Importer.compareFunc);

    // LOD控制
    glSamplerParameterf(mSamplerID, GL_TEXTURE_MIN_LOD, Importer.lodMin);
    glSamplerParameterf(mSamplerID, GL_TEXTURE_MAX_LOD, Importer.lodMax);
    glSamplerParameterf(mSamplerID, GL_TEXTURE_LOD_BIAS, Importer.lodBias);

    // 各向异性过滤
    if (IsAnisotropicFilteringSupported())
    {
        GLfloat maxSupported = 1.0f;
        glGetFloatv(0x84FF, &maxSupported); // 0x84FF = GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
        glSamplerParameterf(mSamplerID, 0x84FE,
                            std::min(Importer.maxAnisotropy, maxSupported)); // 0x84FE = GL_TEXTURE_MAX_ANISOTROPY_EXT
    }

    // 边框颜色（当wrap模式为GL_CLAMP_TO_BORDER时）
    glSamplerParameterfv(mSamplerID, GL_TEXTURE_BORDER_COLOR, Importer.borderColor);
}
} // namespace MEngine