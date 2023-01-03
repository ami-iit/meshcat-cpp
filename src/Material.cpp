/**
 * @file Material.cpp
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#include <MeshcatCpp/Material.h>

using namespace MeshcatCpp;

void Material::set_color(uint8_t r, uint8_t g, uint8_t b)
{
    this->color = static_cast<int>(r << 16) + static_cast<int>(g << 8) + static_cast<int>(b);
}
