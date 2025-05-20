#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "Type.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>

namespace MEngine
{
using Vec2 = Property<glm::vec2>;
using Vec3 = Property<glm::vec3>;
using Vec4 = Property<glm::vec4>;
using Mat4 = Property<glm::mat4>;
using Quat = Property<glm::quat>;
using Mat3 = Property<glm::mat3>;
using Mat2 = Property<glm::mat2>;
using Mat4 = Property<glm::mat4>;
} // namespace MEngine
