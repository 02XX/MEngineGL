#include "Entity/Texture2D.hpp"
#include "Entity/IMaterial.hpp"
#include "Logger.hpp"

namespace MEngine
{
Texture2D::Texture2D()
{
    mName = "Texture2D";
    glCreateTextures(GL_TEXTURE_2D, 1, &mTextureID);
}
Texture2D::~Texture2D()
{
    glDeleteTextures(1, &mTextureID);
}
void Texture2D::SetImagePath(const std::filesystem::path &path)
{
    mImagePath = path;
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
            glTextureSubImage2D(mTextureID, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
            if (mMipLevels > 1)
            {
                glGenerateTextureMipmap(mTextureID);
            }
        }
        else
        {
            LogError("Failed to load texture: {}", mImagePath.string());
        }
    }
}
void Texture2D::SetSampler()
{
    glCreateSamplers(1, &mSamplerID);
    // 设置缩小过滤器（包含mipmap）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_MIN_FILTER, (GLint)mMinFilter);

    // 设置放大过滤器
    glSamplerParameteri(mSamplerID, GL_TEXTURE_MAG_FILTER, (GLint)mMagFilter);

    // 设置纹理环绕方式
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_S, (GLint)mWrapR); // S轴（U）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_T, (GLint)mWrapT); // T轴（V）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_WRAP_R, (GLint)mWrapR); // R轴（3D纹理需要）

    // 比较模式（用于阴影贴图）
    glSamplerParameteri(mSamplerID, GL_TEXTURE_COMPARE_MODE, (GLint)mCompareMode);
    glSamplerParameteri(mSamplerID, GL_TEXTURE_COMPARE_FUNC, (GLint)mCompareFunc);

    // LOD控制
    glSamplerParameterf(mSamplerID, GL_TEXTURE_MIN_LOD, mLodMin);
    glSamplerParameterf(mSamplerID, GL_TEXTURE_MAX_LOD, mLodMax);
    glSamplerParameterf(mSamplerID, GL_TEXTURE_LOD_BIAS, mLodBias);

    // 各向异性过滤
    if (IsAnisotropicFilteringSupported())
    {
        GLfloat maxSupported = 1.0f;
        glGetFloatv(0x84FF, &maxSupported); // 0x84FF = GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
        glSamplerParameterf(mSamplerID, 0x84FE,
                            std::min(mMaxAnisotropy, maxSupported)); // 0x84FE = GL_TEXTURE_MAX_ANISOTROPY_EXT
    }

    // 边框颜色（当wrap模式为GL_CLAMP_TO_BORDER时）
    glSamplerParameterfv(mSamplerID, GL_TEXTURE_BORDER_COLOR, mBorderColor);
}
} // namespace MEngine