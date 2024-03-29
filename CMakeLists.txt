# Authors: Giulio Romualdi

# Set cmake mimimun version
cmake_minimum_required(VERSION 3.12)

project(MeshcatCpp
  VERSION 0.1.0)

# Avoid warnings with CMake 3.26
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# ouptut paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# Build shared libs
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" ON)

# Disable C and C++ compiler extensions.
# C/CXX_EXTENSIONS are ON by default to allow the compilers to use extended
# variants of the C/CXX language.
# However, this could expose cross-platform bugs in user code or in the headers
# of third-party dependencies and thus it is strongly suggested to turn
# extensions off.
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# add GNU dirs
include(GNUInstallDirs)

include(CMakePackageConfigHelpers)

option(ENABLE_RPATH "Enable RPATH for this library" ON)
mark_as_advanced(ENABLE_RPATH)
include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
  LIB_DIRS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
  DEPENDS ENABLE_RPATH
  USE_LINK_PATH)

# Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Release.
if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
  endif()
endif()



find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
include(MeshcatCppDeps)


set(${PROJECT_NAME}_SRC
  src/Meshcat.cpp
  src/Material.cpp
  src/MsgpackTypes.cpp
  src/UUIDGenerator.cpp
  src/Shape.cpp)

set(${PROJECT_NAME}_HDR
  include/MeshcatCpp/Meshcat.h
  include/MeshcatCpp/Material.h
  include/MeshcatCpp/MatrixView.h
  include/MeshcatCpp/Shape.h)

cmrc_add_resource_library(${PROJECT_NAME}_resources
  ALIAS ${PROJECT_NAME}::resources
  NAMESPACE ${PROJECT_NAME}
  misc/index.html
  misc/main.min.js
  misc/favicon.ico)

set_property(TARGET ${PROJECT_NAME}_resources PROPERTY POSITION_INDEPENDENT_CODE ON)


add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRC} ${${PROJECT_NAME}_HDR})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/src;${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/uSockets/src>"
  "$<INSTALL_INTERFACE:>")

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
  target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
else()
  target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
  stduuid
  Threads::Threads
  ZLIB::ZLIB
  msgpack-cxx
  uWebSockets::uWebSockets
  ${PROJECT_NAME}::resources)


set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  PUBLIC_HEADER "${${PROJECT_NAME}_HDR}"
  )

install(TARGETS ${PROJECT_NAME}
  EXPORT  ${PROJECT_NAME}
  COMPONENT runtime
  LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT shlib
  ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT lib
  RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                            COMPONENT bin
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")

include(InstallBasicPackageFiles)
install_basic_package_files(${PROJECT_NAME}
  NAMESPACE MeshcatCpp::
  VERSION ${${PROJECT_NAME}_VERSION}
  COMPATIBILITY SameMajorVersion
  VARS_PREFIX ${PROJECT_NAME}
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

include(AddUninstallTarget)

# Build examples
option(MESHCAT_CPP_BUILT_EXAMPLES "Build the examples" OFF)
if(${MESHCAT_CPP_BUILT_EXAMPLES})
  add_subdirectory(examples)
endif()
