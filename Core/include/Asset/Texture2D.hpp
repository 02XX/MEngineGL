#pragma once
#include "Asset/Texture.hpp"
namespace MEngine
{
enum class Format
{
    R8,
    R8G8,
    R16G16B16,
    R16G16B16A16,
};
class Texture2D final : public Texture
{
  public:
    Format Format = Format::R16G16B16A16;

  public:
    Texture2D();
    ~Texture2D();
};

} // namespace MEngine
namespace nlohmann
{
template <> struct adl_serializer<MEngine::Texture2D>
{
    static void to_json(json &j, const MEngine::Texture2D &texture)
    {
        j = static_cast<MEngine::Texture>(texture);
        j["Format"] = magic_enum::enum_name(texture.Format);
    }
    static void from_json(const json &j, MEngine::Texture2D &texture)
    {
        static_cast<MEngine::Texture &>(texture) = j;
        auto format = j["Format"].get<std::string>();
        auto enumValue = magic_enum::enum_cast<MEngine::Format>(format);
        if (enumValue.has_value())
        {
            texture.Format = enumValue.value();
        }
        else
        {
            throw std::runtime_error("Invalid format value");
        }
    }
};
} // namespace nlohmann
