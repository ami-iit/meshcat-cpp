<p align="center">
<h1 align="center">meshcat-cpp</h1>
</p>
<p align="center">
<a href="https://isocpp.org"><img src="https://img.shields.io/badge/standard-C++17-blue.svg?style=flat&logo=c%2B%2B" alt="C++ Standard"/></a>
<a href="./LICENSE"><img src="https://img.shields.io/badge/License-BSD_3--Clause-orange.svg" alt="Size" /></a>
</p>

Self-contained C++ interface for the [MeshCat visualizer](https://github.com/rdeits/meshcat). 

**meshcat-cpp** took (heavy) inspiration for C++ (drake) MeshCat interface extensively discussed [here](https://github.com/RobotLocomotion/drake/issues/13038). The main purpose of this package is to have an independent (self-contained) C++ library that exposes the MeshCat interface in C++. 

---

<p align="center">
  <b> 🚧 REPOSITORY UNDER DEVELOPMENT 🚧 </b>
  <br>The library implemented in this repository is still experimental and we cannot guarantee stable API
</p>

---

## 🏗️ Dependencies

**meshcat-cpp** is a self-contained library. Most dependencies can be cloned at compile time and they are statically linked to the library. The only dependencies you need is a sufficiently recent C++ compiler (full support to C++17), `cmake`, `openssl`, `zlib`, `libuv`, `boost` and `pkg-config`.

### Install dependencies on Debian/Ubuntu

~~~
sudo apt install cmake pkg-config build-essential libssl-dev libuv1-dev libz-dev libboost-dev
~~~

### Install dependencies with conda-forge

~~~
mamba create -n meshcatcppdev boost-cpp libuwebsockets cmake pkg-config compilers zlib openssl libuv msgpack-cxx
~~~

Then, execute all the other commands after activating the environment:
~~~
mamba activate meshcatcppdev
~~~

## ⚒️ Build the library

You can build the library coping and paste the following snippet into a terminal
```console
git clone https://github.com/GiulioRomualdi/meshcat-cpp.git
cd meshcat-cpp
mkdir build && cd build
cmake ..
cmake --build .
[sudo] make install
```
## 🏃 Example

**meshcat-cpp** provides native `CMake` support which allows the library to be easily used in `CMake` projects. Please add in your `CMakeLists.txt`
```cmake
project(foo)
find_package(MeshcatCpp REQUIRED)
add_executable(${PROJECT_NAME} src/foo.cpp)
target_link_libraries(${PROJECT_NAME} MeshcatCpp::MeshcatCpp)
```

Differently for the [python](https://github.com/rdeits/meshcat-python),
[julia](https://github.com/rdeits/MeshCat.jl), and [C++
(drake)](https://drake.mit.edu/doxygen_cxx/classdrake_1_1geometry_1_1_meshcat_visualizer.html)
interfaces this interface currently supports only a subset of functionalities (🚧 New
functionalities will be soon implemented). The following example shows you how to display some primary shapes.
```cpp
#include <MeshcatCpp/Material.h>
#include <MeshcatCpp/Meshcat.h>
#include <MeshcatCpp/Shape.h>

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
```

Once you have run the [example](./examples/meshcat_example.cpp), the `MeshcatCpp::Meshcat` class will print the `URL` at which the MeshCat server runs. Please open the link in your browser and you should be able to see the following screen

![meshcat_screen](https://user-images.githubusercontent.com/16744101/211311137-3271f266-1a65-4be8-9bd9-230f32bd2d83.png)


## 🐛 Bug reports and support

All types of [issues](https://github.com/GiulioRomualdi/meshcat-cpp/issues/new) are welcome.

## 📝 License
Materials in this repository are distributed under the following license:

> All software is licensed under the BSD 3-Clause "New" or "Revised" License. See [LICENSE](https://github.com/GiulioRomualdi/meshcat-cpp/blob/master/LICENSE) file for details.
