/**
 * @file MsgpackTypes.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_MSGPACK_TYPES_H
#define MESHCAT_CPP_MSGPACK_TYPES_H

#include <MeshcatCpp/Material.h>
#include <MeshcatCpp/MatrixView.h>
#include <MeshcatCpp/Shape.h>

#include <memory>

#include <msgpack.hpp>

#define PACK_MAP_VAR(packer, var) \
    packer.pack(#var);            \
    packer.pack(var);

#define PACK_MAP_VAR_WITH_NAME(packer, name, var) \
    packer.pack(#name);                           \
    packer.pack(var);

#define PACK_MAP_VAR_FROM_INNER_CLASS(packer, inner_class, var) \
    PACK_MAP_VAR_WITH_NAME(packer, var, inner_class.var);

#define PACK_MAP_OPTIONAL_VAR_WITH_NAME(packer, name, var) \
    if (var)                                               \
    {                                                      \
        PACK_MAP_VAR_WITH_NAME(packer, name, var.value()); \
    }

#define PACK_MAP_OPTIONAL_VAR_FROM_INNER_CLASS(packer, inner_class, var) \
    PACK_MAP_OPTIONAL_VAR_WITH_NAME(packer, var, inner_class.var);

#define SHAPE_TRAMPOLINE(shape)                    \
    struct shape##Trampoline;                      \
    template <> struct traits<::MeshcatCpp::shape> \
    {                                              \
        using trampoline = shape##Trampoline;      \
    };

namespace MeshcatCpp::details
{

template <typename T> struct traits;

/// @note the following is from the Eigen library
/// here we say once and for all that traits<const T> == traits<T>
///
/// When constness must affect traits, it has to be constness on
/// template parameters on which T itself depends.
/// For example, traits<Map<const T> > != traits<Map<T> >, but
///              traits<const Map<T> > == traits<Map<T> >
template <typename T> struct traits<const T> : traits<T>
{
};

struct MaterialTrampoline
{
    const std::string uuid;
    const ::MeshcatCpp::Material material;

    MaterialTrampoline(const ::MeshcatCpp::Material& material);

    template <typename Packer> void msgpack_pack(Packer& o) const
    {
        int n = 4;
        if (material.linewidth)
            ++n;
        if (material.opacity)
            ++n;
        if (material.reflectivity)
            ++n;
        if (material.side)
            ++n;
        if (material.transparent)
            ++n;
        if (material.wireframe)
            ++n;
        if (material.wireframeLineWidth)
            ++n;

        o.pack_map(n);
        PACK_MAP_VAR(o, uuid);

        PACK_MAP_VAR_WITH_NAME(o, type, material.type_map.at(material.type));
        PACK_MAP_VAR_WITH_NAME(o, color, material.color);
        PACK_MAP_VAR_WITH_NAME(o, vertexColors, material.vertexColors);

        PACK_MAP_OPTIONAL_VAR_FROM_INNER_CLASS(o, material, linewidth);
        PACK_MAP_OPTIONAL_VAR_FROM_INNER_CLASS(o, material, opacity);
        PACK_MAP_OPTIONAL_VAR_FROM_INNER_CLASS(o, material, reflectivity);
        PACK_MAP_OPTIONAL_VAR_FROM_INNER_CLASS(o, material, side);
        PACK_MAP_OPTIONAL_VAR_FROM_INNER_CLASS(o, material, transparent);
        PACK_MAP_OPTIONAL_VAR_FROM_INNER_CLASS(o, material, wireframe);
        PACK_MAP_OPTIONAL_VAR_FROM_INNER_CLASS(o, material, wireframeLineWidth);
    }

    // This method must be defined, but the implementation is not needed.
    void msgpack_unpack(msgpack::object const&);

};

struct ObjectMetaData
{
    const std::string type{"Object"};
    const double version{4.5};

    template <typename Packer>

    void msgpack_pack(Packer& o) const
    {
        constexpr int n = 2;
        o.pack_map(n);
        PACK_MAP_VAR(o, type);
        PACK_MAP_VAR(o, version);
    }

    // This method must be defined, but the implementation is not needed.
    void msgpack_unpack(msgpack::object const&)
    {
        throw std::runtime_error("unpack is not implemented for ObjectMetaData.");
    }
};

struct GeometryData
{
    virtual ~GeometryData() = default;

    GeometryData();

    const std::string uuid;

    // NOLINTNEXTLINE(runtime/references) cpplint disapproves of msgpack choices.
    virtual void msgpack_pack(msgpack::packer<std::stringstream>& o) const = 0;

    // This method must be defined, but the implementation is not needed in the
    // current workflows.
    void msgpack_unpack(msgpack::object const&);
};

SHAPE_TRAMPOLINE(Sphere);
struct SphereTrampoline : public GeometryData
{
    const ::MeshcatCpp::Sphere sphere;
    double widthSegments{20};
    double heightSegments{20};

    SphereTrampoline(const ::MeshcatCpp::Sphere& sphere);

    void msgpack_pack(msgpack::packer<std::stringstream>& o) const override;
};

SHAPE_TRAMPOLINE(Ellipsoid);
struct EllipsoidTrampoline : public GeometryData
{
    const ::MeshcatCpp::Ellipsoid ellipsoid;
    double widthSegments{20};
    double heightSegments{20};

    EllipsoidTrampoline(const ::MeshcatCpp::Ellipsoid& ellipsoid);

    void msgpack_pack(msgpack::packer<std::stringstream>& o) const override;
};

SHAPE_TRAMPOLINE(Box);
struct BoxTrampoline : public GeometryData
{
    const ::MeshcatCpp::Box box;
    double widthSegments{20};
    double heightSegments{20};

    BoxTrampoline(const ::MeshcatCpp::Box& sphere);

    void msgpack_pack(msgpack::packer<std::stringstream>& o) const override;
};

SHAPE_TRAMPOLINE(Cylinder);
struct CylinderTrampoline : public GeometryData
{
    const ::MeshcatCpp::Cylinder cylinder;
    double radialSegments{50};
    double heightSegments{20};

    CylinderTrampoline(const ::MeshcatCpp::Cylinder& cylinder);

    void msgpack_pack(msgpack::packer<std::stringstream>& o) const override;
};

SHAPE_TRAMPOLINE(Mesh);
struct MeshTrampoline : public GeometryData
{
    const ::MeshcatCpp::Mesh mesh;

    std::string format;
    std::vector<char> data;

    MeshTrampoline(const ::MeshcatCpp::Mesh& mesh);

    void msgpack_pack(msgpack::packer<std::stringstream>& o) const override;
};

struct MeshData
{
    std::string uuid;
    std::string type{"Mesh"};
    std::string geometry;
    std::string material;
    std::array<double, 16> matrix_vec = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    // TODO it cannot be copiable
    MeshData();

    MeshcatCpp::MatrixView<double> matrix();

    template<typename T>
    void update_matrix_from_shape(const T& shape)
    {
        if constexpr (std::is_same_v<T, ::MeshcatCpp::Ellipsoid>)
        {
            auto view = this->matrix();
            view(0, 0) = shape.a();
            view(1, 1) = shape.b();
            view(2, 2) = shape.c();
        }

        if constexpr (std::is_same_v<T, ::MeshcatCpp::Mesh>)
        {
            auto view = this->matrix();
            view(0, 0) = view(1, 1) = view(2, 2) = shape.scale();
        }
    }

    MSGPACK_DEFINE_MAP(uuid, type, geometry, material, MSGPACK_NVP("matrix", matrix_vec));
};

struct LumpedObjectData
{
    ObjectMetaData metadata;
    std::unique_ptr<GeometryData> geometry;
    std::unique_ptr<MaterialTrampoline> material;
    MeshData object;

    template <typename Packer> void msgpack_pack(Packer& o) const
    {
        int size = 2;
        if (geometry)
        {
            size++;
        }
        if (material)
        {
            size++;
        }

        o.pack_map(size);
        PACK_MAP_VAR(o, metadata);

        if (geometry)
        {
            o.pack("geometries");
            o.pack_array(1);
            o.pack(*geometry);
        }
        if (material)
        {
            o.pack("materials");
            o.pack_array(1);
            o.pack(*material);
        }

        o.pack("object");
        o.pack(object);
    }

    // This method must be defined, but the implementation is not needed
    void msgpack_unpack(msgpack::object const&)
    {
        throw std::runtime_error("unpack is not implemented for LumpedObjectData.");
    }
};

struct SetObjectData
{
    std::string type{"set_object"};
    std::string path;
    LumpedObjectData object;
    MSGPACK_DEFINE_MAP(type, path, object);
};

} // namespace MeshcatCpp::details

#endif // MESHCAT_CPP_MSGPACK_TYPES_H
