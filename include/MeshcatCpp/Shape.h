/**
 * @file Shape.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_SHAPE_H
#define MESHCAT_CPP_SHAPE_H

#include <string>

#define MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(type, attribute) \
private:                                                 \
    type attribute##_;                                   \
                                                         \
public:                                                  \
    const type& attribute() const                        \
    {                                                    \
        return this->attribute##_;                       \
    }

namespace MeshcatCpp
{

struct Shape
{
    const std::string type;
};

class Sphere : public Shape
{
public:
    Sphere(double radius);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, radius);
};

class Ellipsoid : public Shape
{
public:
    Ellipsoid(double a, double b, double c);

    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, a);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, b);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, c);
};

class Cylinder : public Shape
{
public:
    Cylinder(double radius, double height);

    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, radius);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, height);
};

class Box : public Shape
{
public:
    Box(double width, double depth, double height);

    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, width);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, depth);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, height);
};

class Mesh : public Shape
{
public:
    Mesh(std::string_view file_path, double scale = 1);

    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(std::string, file_path);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(double, scale);
};


} // namespace MeshcatCpp

#undef MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE

#endif // MESHCAT_CPP_SHAPE_H
