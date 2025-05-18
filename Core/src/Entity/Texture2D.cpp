#include "Entity/Texture2D.hpp"
#include "Entity/IMaterial.hpp"
#include "Logger.hpp"

namespace MEngine
{
Texture2D::Texture2D()
{
    mName = "Texture2D";
    glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);
    GL_CHECK();
    glCreateSamplers(1, &mSamplerID);
    GL_CHECK();
}
Texture2D::~Texture2D()
{
    glDeleteTextures(1, &mTextureID);
    glDeleteSamplers(1, &mSamplerID);
}
void Texture2D::SetImagePath(const std::filesystem::path &path)
{
    mImagePath = path;
}
void Texture2D::Update()
{
    if (std::filesystem::exists(mImagePath))
    {
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data =
            stbi_load(mImagePath.string().c_str(), (int *)&mWidth, (int *)&mHeight, (int *)&mChannels, STBI_rgb_alpha);
        if (data)
        {
            mMipLevels =
                std::min(1 + static_cast<GLsizei>(std::floor(std::log2(std::max(mWidth, mHeight)))), mMipLevels);
            glTextureStorage2D(mTextureID, mMipLevels, (GLenum)mInternalFormat, mWidth, mHeight);
            GL_CHECK();
            glTextureSubImage2D(mTextureID, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
            GL_CHECK();
            if (mMipLevels > 1)
            {
                glGenerateTextureMipmap(mTextureID);
                GL_CHECK();
            }
            stbi_image_free(data);
        }
        else
        {
            LogError("Failed to load texture: {}", mImagePath.string());
        }
    }
    else // TODO: 默认纹理
    {
        LogError("Texture file does not exist: {}, 记得后续重构添加默认纹理", mImagePath.string());
        return;
    }
    GL_CHECK();
    // 设置缩小过滤器（包含mipmap）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, (GLint)mImporter.minFilter);

    // 设置放大过滤器
    glSamplerParameteri(mSamplerID, GL_TEXTURE_MAG_FILTER, (GLint)mImporter.magFilter);

    // 设置纹理环绕方式
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_S, (GLint)mImporter.wrapR); // S轴（U）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_T, (GLint)mImporter.wrapT); // T轴（V）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_R, (GLint)mImporter.wrapR); // R轴（3D纹理需要）

    // 比较模式（用于阴影贴图）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_COMPARE_MODE, (GLint)mImporter.compareMode);
    glSamplerParameteri(mSamplerID, GL_TEXTURE_COMPARE_FUNC, (GLint)mImporter.compareFunc);

    // LOD控制
    glSamplerParameterf(mSamplerID, GL_TEXTURE_MIN_LOD, mImporter.lodMin);
    glSamplerParameterf(mSamplerID, GL_TEXTURE_MAX_LOD, mImporter.lodMax);
    glSamplerParameterf(mSamplerID, GL_TEXTURE_LOD_BIAS, mImporter.lodBias);

    // 各向异性过滤
    if (IsAnisotropicFilteringSupported())
    {
        GLfloat maxSupported = 1.0f;
        glGetFloatv(0x84FF, &maxSupported); // 0x84FF = GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
        glSamplerParameterf(mSamplerID, 0x84FE,
                            std::min(mImporter.maxAnisotropy, maxSupported)); // 0x84FE = GL_TEXTURE_MAX_ANISOTROPY_EXT
    }

    // 边框颜色（当wrap模式为GL_CLAMP_TO_BORDER时）
    glSamplerParameterfv(mSamplerID, GL_TEXTURE_BORDER_COLOR, mImporter.borderColor);
    GL_CHECK();
}
} // namespace MEngine