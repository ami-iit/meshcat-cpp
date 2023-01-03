/**
 * @file Shape.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_SHAPE_H
#define MESHCAT_CPP_SHAPE_H

#include <string>

#define MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(attribute) \
private:                                           \
    double attribute##_;                           \
                                                   \
public:                                            \
    const double& attribute() const                \
    {                                              \
        return this->attribute##_;                 \
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
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(radius);
};

class Ellipsoid : public Shape
{
public:
    Ellipsoid(double a, double b, double c);

    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(a);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(b);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(c);
};

class Cylinder : public Shape
{
public:
    Cylinder(double radius, double height);

    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(radius);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(height);
};


class Box : public Shape
{
public:
    Box(double width, double depth, double height);

    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(width);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(depth);
    MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE(height);
};

} // namespace MeshcatCpp

#undef MESHCAT_CPP_ADD_SHAPE_ATTRIBUTE

#endif // MESHCAT_CPP_SHAPE_H
