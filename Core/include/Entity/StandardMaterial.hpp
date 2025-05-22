//
// Created by 02 on 25-5-22.
//

#pragma  once
#include "Material.hpp"

namespace MEngine {

class StandardMaterial final: public Material{
    public:
    StandardMaterial();
};

} // MEngine

namespace nlohmann
{
    template <> struct adl_serializer<MEngine::StandardMaterial>
    {
        static void to_json(json &j, const MEngine::StandardMaterial &material)
        {
            j = static_cast<MEngine::Material>(material);

        }
        static void from_json(const json &j, MEngine::Material &material)
        {
            static_cast<MEngine::Material&>(material) = j;
        }
    };
}
