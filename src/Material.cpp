/**
 * @file Material.cpp
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#include <MeshcatCpp/Material.h>

using namespace MeshcatCpp;

void Material::set_color(uint8_t r, uint8_t g, uint8_t b, double a)
{
    this->color = static_cast<int>(r << 16) + static_cast<int>(g << 8) + static_cast<int>(b);
    this->opacity = a;
    if (this->opacity < 1.0)
    {
        this->transparent = true;
    }
}

Material Material::get_default_material()
{
    Material tmp{.reflectivity = 0.5,
                 .side = 2,
                 .transparent = false,
                 .opacity = 1.0,
                 .linewidth = 1.0,
                 .wireframe = false,
                 .wireframeLineWidth = 1.0,
                 .vertexColors = false,
                 .type = Material::Type::MeshPhongMaterial};

    return tmp;
}
