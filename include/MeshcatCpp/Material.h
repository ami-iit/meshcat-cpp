/**
 * @file Material.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_MATERIAL_H
#define MESHCAT_CPP_MATERIAL_H


#include <string>
#include <unordered_map>
#include <utility>

namespace MeshcatCpp
{

struct Material
{
public:
    enum class Type
    {
        MeshBasicMaterial,
        MeshPhongMaterial,
        MeshLambertMaterial,
        MeshToonMaterial,
        LineBasicMaterial
    };

    int color{(229 << 16) + (229 << 8) + 229};
    std::optional<double> reflectivity;
    std::optional<int> side;
    std::optional<bool> transparent;
    std::optional<double> opacity;
    std::optional<double> linewidth;
    std::optional<bool> wireframe;
    std::optional<double> wireframeLineWidth;
    bool vertexColors{false};
    Type type{Type::MeshPhongMaterial};

    const std::unordered_map<Type, std::string> type_map{{Type::MeshBasicMaterial, "MeshBasicMaterial"},
                                                         {Type::MeshPhongMaterial, "MeshPhongMaterial"},
                                                         {Type::MeshLambertMaterial, "MeshLambertMaterial"},
                                                         {Type::MeshToonMaterial, "MeshToonMaterial"},
                                                         {Type::LineBasicMaterial, "LineBasicMaterial"}};


    void set_color(uint8_t r, uint8_t g, uint8_t b);

    static Material get_default_material();
};

} // namespace MeshcatCpp

#endif // MESHCAT_CPP_MATERIAL_H
