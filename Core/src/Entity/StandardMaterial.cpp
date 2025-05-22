//
// Created by 02 on 25-5-22.
//

#include "Entity/StandardMaterial.hpp"

namespace MEngine {
    StandardMaterial::StandardMaterial()
    {
        Name = "StandardMaterial";
        MaterialType = MaterialType::PBR;
        Type = EntityType::Material;
    }
} // MEngine