/**
 * @file meshcat_example.cpp
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#include <MeshcatCpp/Material.h>
#include <MeshcatCpp/Meshcat.h>
#include <MeshcatCpp/Shape.h>

MeshcatCpp::MatrixView<double> array_to_matrix_view(std::array<double, 16>& array)
{
    constexpr MeshcatCpp::MatrixView<double>::index_type rows = 4;
    constexpr MeshcatCpp::MatrixView<double>::index_type cols = 4;
    return MeshcatCpp::make_matrix_view(array.data(),
                                        rows,
                                        cols,
                                        MeshcatCpp::MatrixView<
                                            double>::MatrixStorageOrdering::ColumnMajor);
}

int main()
{
    MeshcatCpp::Meshcat meshcat;
    MeshcatCpp::Material m;

    std::array<double, 16> transform = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    auto matrix_view = array_to_matrix_view(transform);

    m.set_color(66, 133, 244);
    meshcat.set_object("box", MeshcatCpp::Box(0.5, 0.5, 0.5), m);
    matrix_view(3, 1) = 1.75;
    meshcat.set_transform("box", matrix_view);

    m.set_color(234, 67, 53);
    meshcat.set_object("sphere", MeshcatCpp::Sphere(0.5), m);
    matrix_view(3, 1) = 0.75;
    meshcat.set_transform("sphere", matrix_view);

    m.set_color(251, 188, 5);
    meshcat.set_object("ellipsoid", MeshcatCpp::Ellipsoid(0.5, 0.25, 0.75), m);
    matrix_view(3, 1) = -0.75;
    meshcat.set_transform("ellipsoid", matrix_view);

    m.set_color(52, 168, 83);
    meshcat.set_object("cylinder", MeshcatCpp::Cylinder(0.25, 0.5), m);
    matrix_view(3, 1) = -1.75;
    meshcat.set_transform("cylinder", matrix_view);

    meshcat.join();

    return 0;
}
