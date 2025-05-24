#pragma once
#include "Asset/Material.hpp"
#include <glm/vec3.hpp>
#include <nlohmann/json.hpp>
#include <variant>

namespace MEngine
{
namespace Core
{
class CustomMaterial final : public Material
{

  public:
    UUID PipelineID{};
    std::vector<std::variant<int, float, glm::vec3>> Parameters;
    std::vector<UUID> Textures;
};
} // namespace Core
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::Core::CustomMaterial>
{
    static void to_json(json &j, const MEngine::Core::CustomMaterial &material)
    {
        j = static_cast<MEngine::Core::Material>(material);
        j["PipelineID"] = material.PipelineID;
        j["Parameters"] = json::array();
        for (const auto &param : material.Parameters)
        {
            if (std::holds_alternative<int>(param))
            {
                j["Parameters"].push_back(std::get<int>(param));
            }
            else if (std::holds_alternative<float>(param))
            {
                j["Parameters"].push_back(std::get<float>(param));
            }
            else if (std::holds_alternative<glm::vec3>(param))
            {
                auto vec = std::get<glm::vec3>(param);
                j["Parameters"].push_back({{"x", vec.x}, {"y", vec.y}, {"z", vec.z}});
            }
            else
            {
                throw std::runtime_error("Unsupported parameter type");
            }
        }
        j["Textures"] = material.Textures;
    }
    static void from_json(const json &j, MEngine::Core::CustomMaterial &material)
    {
        static_cast<MEngine::Core::Material &>(material) = j;
        material.PipelineID = j.at("PipelineID").get<MEngine::Core::UUID>();
        material.Parameters.clear();
        for (const auto &param : j.at("Parameters"))
        {
            if (param.is_number_integer())
            {
                material.Parameters.push_back(param.get<int>());
            }
            else if (param.is_number_float())
            {
                material.Parameters.push_back(param.get<float>());
            }
            else if (param.is_object())
            {
                glm::vec3 vec;
                vec.x = param.at("x").get<float>();
                vec.y = param.at("y").get<float>();
                vec.z = param.at("z").get<float>();
                material.Parameters.push_back(vec);
            }
            else
            {
                throw std::runtime_error("Unsupported parameter type");
            }
        }
        material.Textures = j.at("Textures").get<std::vector<MEngine::Core::UUID>>();
    }
};

} // namespace nlohmann
