/**
 * @file Meshcat.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

// Part of the content of this file has been taken from drake. Drake lincense follows
//
// All components of Drake are licensed under the BSD 3-Clause License
// shown below. Where noted in the source code, some portions may
// be subject to other permissive, non-viral licenses.
//
// Copyright 2012-2022 Robot Locomotion Group @ CSAIL
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.  Redistributions
// in binary form must reproduce the above copyright notice, this list of
// conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.  Neither the name of
// the Massachusetts Institute of Technology nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

    void set_property(std::string_view path, const std::string& property, bool value);

    void
    set_object(std::string_view path, const Sphere& sphere, const Material& material = Material::get_default_material());

    void set_object(std::string_view path,
                    const Cylinder& cylinder,
                    const Material& material = Material::get_default_material());

    void set_object(std::string_view path, const Box& box, const Material& material = Material::get_default_material());

    void set_object(std::string_view path,
                    const Ellipsoid& ellipsoid,
                    const Material& material = Material::get_default_material());

    void set_object(std::string_view path, const Mesh& mesh, const Material& material = Material::get_default_material());

    void set_transform(std::string_view path, const MatrixView<const double>& matrix);
private:

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace MeshcatCpp

#endif // MESHCAT_CPP_MESHCAT_H
