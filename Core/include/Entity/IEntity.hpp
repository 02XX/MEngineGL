#pragma once

#include "Logger.hpp"
#include "UUID.hpp"
#include <filesystem>
#include <glad/glad.h>
#include <nlohmann/json.hpp>

namespace MEngine
{
#define GL_CHECK()                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        bool hasError = false;                                                                                         \
        GLenum err;                                                                                                    \
        while ((err = glGetError()) != GL_NO_ERROR)                                                                    \
        {                                                                                                              \
            hasError = true;                                                                                           \
            std::string errorMsg;                                                                                      \
            switch (err)                                                                                               \
            {                                                                                                          \
            case GL_INVALID_OPERATION:                                                                                 \
                errorMsg = "GL_INVALID_OPERATION";                                                                     \
                break;                                                                                                 \
            case GL_INVALID_VALUE:                                                                                     \
                errorMsg = "GL_INVALID_VALUE";                                                                         \
                break;                                                                                                 \
            case GL_INVALID_ENUM:                                                                                      \
                errorMsg = "GL_INVALID_ENUM";                                                                          \
                break;                                                                                                 \
            case GL_OUT_OF_MEMORY:                                                                                     \
                errorMsg = "GL_OUT_OF_MEMORY";                                                                         \
                break;                                                                                                 \
            case GL_INVALID_FRAMEBUFFER_OPERATION:                                                                     \
                errorMsg = "GL_INVALID_FRAMEBUFFER_OPERATION";                                                         \
                break;                                                                                                 \
            default:                                                                                                   \
                errorMsg = "Unknown error code: " + std::to_string(err);                                               \
                break;                                                                                                 \
            }                                                                                                          \
            LogError("[OpenGL Error] {} at {}:{} in {}", errorMsg, __FILE__, __LINE__, __FUNCTION__);                  \
        }                                                                                                              \
    } while (false)
class IEntity
{
  public:
    virtual ~IEntity() = default;
    virtual const UUID &GetID() const = 0;
    virtual const std::filesystem::path &GetPath() const = 0;
    virtual const std::string &GetName() const = 0;
    virtual void SetID(const UUID &id) = 0;
    virtual void SetPath(const std::filesystem::path &path) = 0;
    virtual void SetName(const std::string &name) = 0;
};
} // namespace MEngine
