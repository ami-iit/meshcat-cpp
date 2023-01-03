/**
 * @file Meshcat.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_MESHCAT_H
#define MESHCAT_CPP_MESHCAT_H

#include <memory>

#include <MeshcatCpp/Material.h>
#include <MeshcatCpp/Shape.h>
#include <MeshcatCpp/MatrixView.h>

namespace MeshcatCpp
{
/**
 * The Meshcat class provides an interface to [meshcat]( https://github.com/rdeits/meshcat).
 * This class's instances start a thread that runs a http/websocket server. Users may view the
 * Meshcat scene by navigating their browser to the hosted URL.
 * Users can exploit this class to load primary shapes (e.g. spheres, cylinders ellipsoids and
 * boxes). For example,
 * @verbatim
 * Meshcat viz;
 * viz.set_object("box", MeshcatCpp::Box(0.1, 0.1, 0.1));
 * @endverbatim
 * will start an interface between meshcat and load a box.
 * @note the Design of this class took inspiration form drake Meshcat C++ implementation.
 * Please refer to https://github.com/RobotLocomotion/drake/issues/13038 if you are interested in
 * the original project.
 */
class Meshcat
{
public:
    Meshcat& operator=(const Meshcat&) = delete;
    Meshcat(const Meshcat&) = delete;

    /**
     * Constructs the Meshcat instance. It will listen on the first available port starting at 7001
     * (up to 7099).
     */
    Meshcat();

    ~Meshcat();

    /**
     * Utility function to make the meshcat interface run forever (until the user stop the
     * application)
     */
    void join();

    /**
     * Forward the set_property command to the meshcat visualizer.
     * Please check https://github.com/rdeits/meshcat/blob/master/Readme.md#api for further
     * informations
     */
    void set_property(std::string_view path, const std::string& property, bool value);

    void
    set_object(std::string_view path, const Sphere& sphere, const Material& material = Material());

    void set_object(std::string_view path,
                    const Cylinder& cylinder,
                    const Material& material = Material());

    void set_object(std::string_view path, const Box& box, const Material& material = Material());

    void set_object(std::string_view path,
                    const Ellipsoid& ellipsoid,
                    const Material& material = Material());

private:

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace MeshcatCpp

#endif // MESHCAT_CPP_MESHCAT_H
