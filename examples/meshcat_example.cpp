/**
 * @file meshcat_example.cpp
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#include <MeshcatCpp/Material.h>
#include <MeshcatCpp/Meshcat.h>
#include <MeshcatCpp/Shape.h>

#include <array>
#include <filesystem>

MeshcatCpp::MatrixView<double> array_to_matrix_view(std::array<double, 16>& array)
{
    constexpr MeshcatCpp::MatrixView<double>::index_type rows = 4;
    constexpr MeshcatCpp::MatrixView<double>::index_type cols = 4;
    constexpr auto order = MeshcatCpp::MatrixStorageOrdering::ColumnMajor;
    return MeshcatCpp::make_matrix_view(array.data(), rows, cols, order);
}

int main()
{
    MeshcatCpp::Meshcat meshcat;
    MeshcatCpp::Material m = MeshcatCpp::Material::get_default_material();

    std::array<double, 16> transform = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    auto matrix_view = array_to_matrix_view(transform);

    m.set_color(66, 133, 244);
    meshcat.set_object("box", MeshcatCpp::Box(0.5, 0.5, 0.5), m);
    matrix_view(1, 3) = 1.75;

    meshcat.set_transform("box", matrix_view);

    m.set_color(234, 67, 53);
    meshcat.set_object("sphere", MeshcatCpp::Sphere(0.5), m);
    matrix_view(1, 3) = 0.75;
    meshcat.set_transform("sphere", matrix_view);

    m.set_color(251, 188, 5);
    meshcat.set_object("ellipsoid", MeshcatCpp::Ellipsoid(0.5, 0.25, 0.75), m);
    matrix_view(1, 3) = -0.75;
    meshcat.set_transform("ellipsoid", matrix_view);

    m.set_color(52, 168, 83);
    meshcat.set_object("cylinder", MeshcatCpp::Cylinder(0.25, 0.5), m);
    matrix_view(1, 3) = -1.75;
    meshcat.set_transform("cylinder", matrix_view);

    const auto stl_path = std::filesystem::path(__FILE__).parent_path() / "misc" / "Dragonite.stl";
    meshcat.set_object("obj", MeshcatCpp::Mesh(stl_path.string(), 0.01));
    matrix_view(0, 3) = -1;
    matrix_view(1, 3) = 0;
    matrix_view(0, 0) = 0;
    matrix_view(1, 1) = 0;
    matrix_view(0, 1) = -1;
    matrix_view(1, 0) = 1;
    meshcat.set_transform("obj", matrix_view);

    meshcat.join();

    return EXIT_SUCCESS;
}
