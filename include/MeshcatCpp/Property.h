/**
 * @file Property.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_PROPERTY_H
#define MESHCAT_CPP_PROPERTY_H

#include <string>

namespace MeshcatCpp
{

template <typename T> struct Property
{
    std::string path;
    std::string property;
    T value;
};

} // namespace MeshcatCpp

#endif // MESHCAT_CPP_MATERIAL_H
