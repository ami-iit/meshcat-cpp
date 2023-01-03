/**
 * @file Material.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_MATERIAL_H
#define MESHCAT_CPP_MATERIAL_H

// #include <msgpack.hpp>

#include <string>
// e#include <MeshcatCpp/GenericElement.h> //
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
    double reflectivity{0.5};
    int side{2};
    bool transparent{false};
    double opacity{1.0};
    double linewidth{1.0};
    bool wireframe{false};
    double wireframeLineWidth{1.0};
    bool vertexColors{false};
    Type type{Type::MeshPhongMaterial};

    std::unordered_map<Type, std::string> typeMap{{Type::MeshBasicMaterial, "MeshBasicMaterial"},
                                                  {Type::MeshPhongMaterial, "MeshPhongMaterial"},
                                                  {Type::MeshLambertMaterial, "MeshLambertMaterial"},
                                                  {Type::MeshToonMaterial, "MeshToonMaterial"},
                                                  {Type::LineBasicMaterial, "LineBasicMaterial"}};
};

} // namespace MeshcatCpp

#endif // MESHCAT_CPP_MATERIAL_H
