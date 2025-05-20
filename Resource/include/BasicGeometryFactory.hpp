
#pragma once
#include "Entity/Mesh.hpp"
#include <vector>

namespace MEngine
{
enum class PrimitiveType
{
    Cube,     // 立方体
    Cylinder, // 圆柱体
    Sphere,   // 球体
    Quad,     // 四边形
};
struct Geometry
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
class BasicGeometryFactory final
{
  public:
    BasicGeometryFactory();

    Geometry GetGeometry(PrimitiveType type);

  private:
    Geometry GetCube();
    Geometry GetCylinder();
    Geometry GetSphere();
    Geometry GetQuad();
};

} // namespace MEngine