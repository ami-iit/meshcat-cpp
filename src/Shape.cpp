/**
 * @file Shape.cpp
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#include <MeshcatCpp/Shape.h>

using namespace MeshcatCpp;

Sphere::Sphere(double radius)
    : Shape{.type = "SphereGeometry"}
    , radius_(std::move(radius))
{
}

Cylinder::Cylinder(double radius, double height)
    : Shape{.type = "CylinderGeometry"}
    , radius_(std::move(radius))
    , height_(std::move(height))
{
}

Box::Box(double width, double depth, double height)
    : Shape{.type = "BoxGeometry"}
    , width_(std::move(width))
    , depth_(std::move(depth))
    , height_(std::move(height))
{
}

Ellipsoid::Ellipsoid(double a, double b, double c)
    : Shape{.type = "SphereGeometry"}
    , a_(std::move(a))
    , b_(std::move(b))
    , c_(std::move(c))
{
}

Mesh::Mesh(std::string_view file_path, double scale)
    : Shape{.type = "_meshfile_geometry"}
    , file_path_(file_path)
    , scale_(std::move(scale))
{
}
