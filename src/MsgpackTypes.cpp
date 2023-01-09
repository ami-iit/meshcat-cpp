/**
 * @file MsgpackTypes.cpp
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#include <MeshcatCpp/impl/MsgpackTypes.h>
#include <MeshcatCpp/impl/UUIDGenerator.h>
#include <MeshcatCpp/MatrixView.h>

#include <filesystem>
#include <fstream>

using namespace MeshcatCpp::details;

MaterialTrampoline::MaterialTrampoline(const ::MeshcatCpp::Material& material)
    : uuid(MeshcatCpp::details::UUIDGenerator::generator()())
    , material(material)
{
}

// This method must be defined, but the implementation is not needed.
void MaterialTrampoline::msgpack_unpack(msgpack::object const&)
{
    throw std::runtime_error("unpack is not implemented for MaterialTrampoline.");
}

GeometryData::GeometryData()
    : uuid(MeshcatCpp::details::UUIDGenerator::generator()())
{
}

void GeometryData::msgpack_unpack(msgpack::object const&)
{
    throw std::runtime_error("unpack is not implemented for BufferGeometryData.");
}

SphereTrampoline::SphereTrampoline(const ::MeshcatCpp::Sphere& sphere)
    : GeometryData()
    , sphere(sphere)
{
}

void SphereTrampoline::msgpack_pack(msgpack::packer<std::stringstream>& o) const
{
    o.pack_map(5);
    PACK_MAP_VAR_FROM_INNER_CLASS(o, sphere, type);
    PACK_MAP_VAR(o, uuid);
    PACK_MAP_VAR_WITH_NAME(o, radius, sphere.radius());
    PACK_MAP_VAR(o, widthSegments);
    PACK_MAP_VAR(o, heightSegments);
}

CylinderTrampoline::CylinderTrampoline(const ::MeshcatCpp::Cylinder& cylinder)
    : GeometryData()
    , cylinder(cylinder)
{
}

void CylinderTrampoline::msgpack_pack(msgpack::packer<std::stringstream>& o) const
{
    o.pack_map(6);
    PACK_MAP_VAR_FROM_INNER_CLASS(o, cylinder, type);
    PACK_MAP_VAR(o, uuid);
    PACK_MAP_VAR_WITH_NAME(o, radiusTop, cylinder.radius());
    PACK_MAP_VAR_WITH_NAME(o, radiusBottom, cylinder.radius());
    PACK_MAP_VAR_WITH_NAME(o, height, cylinder.height());
    PACK_MAP_VAR(o, radialSegments);
}

BoxTrampoline::BoxTrampoline(const ::MeshcatCpp::Box& box)
    : GeometryData()
    , box(box)
{
}

void BoxTrampoline::msgpack_pack(msgpack::packer<std::stringstream>& o) const
{
    o.pack_map(5);
    PACK_MAP_VAR_FROM_INNER_CLASS(o, box, type);
    PACK_MAP_VAR(o, uuid);
    PACK_MAP_VAR_WITH_NAME(o, width, box.width());
    PACK_MAP_VAR_WITH_NAME(o, height, box.depth());
    PACK_MAP_VAR_WITH_NAME(o, depth, box.height());
}

EllipsoidTrampoline::EllipsoidTrampoline(const ::MeshcatCpp::Ellipsoid& ellipsoid)
    : GeometryData()
    , ellipsoid(ellipsoid)
{
}

void EllipsoidTrampoline::msgpack_pack(msgpack::packer<std::stringstream>& o) const
{
    constexpr int radius = 1;
    o.pack_map(5);
    PACK_MAP_VAR_FROM_INNER_CLASS(o, ellipsoid, type);
    PACK_MAP_VAR(o, uuid);
    PACK_MAP_VAR_WITH_NAME(o, radius, radius);
    PACK_MAP_VAR(o, widthSegments);
    PACK_MAP_VAR(o, heightSegments);
}

MeshTrampoline::MeshTrampoline(const ::MeshcatCpp::Mesh& mesh)
    : GeometryData()
    , mesh(mesh)
{
    const auto& path = mesh.file_path();
    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos)
    {
        return;
    }
    this->format = path.substr(pos + 1);

    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input.is_open())
    {
        return;
    }

    const int size = input.tellg();

    input.seekg(0, std::ios::beg);
    this->data.resize(size);
    input.read(this->data.data(), size);

    input.close();
}

void MeshTrampoline::msgpack_pack(msgpack::packer<std::stringstream>& o) const
{
    o.pack_map(4);
    PACK_MAP_VAR_FROM_INNER_CLASS(o, mesh, type);
    PACK_MAP_VAR(o, uuid);
    PACK_MAP_VAR(o, format);
    PACK_MAP_VAR(o, data);
}

MeshData::MeshData()
    : uuid(MeshcatCpp::details::UUIDGenerator::generator()())
{
}

::MeshcatCpp::MatrixView<double> MeshData::matrix()
{
    constexpr MeshcatCpp::MatrixView<double>::index_type rows = 4;
    constexpr MeshcatCpp::MatrixView<double>::index_type cols = 4;
    return ::MeshcatCpp::make_matrix_view(this->matrix_vec.data(),
                                          rows,
                                          cols,
                                          ::MeshcatCpp::MatrixStorageOrdering::ColumnMajor);
}
