# Authors: Giulio Romualdi

# Set cmake mimimun version
cmake_minimum_required(VERSION 3.5)

project(MeshcatCpp
  VERSION 0.1.0)

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

## compile uWebSocket
add_custom_target(uWebSockets COMMAND make all
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets)

set(${PROJECT_NAME}_SRC
  src/Meshcat.cpp)

set(${PROJECT_NAME}_HDR
  include/MeshcatCpp/Meshcat.h)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRC} ${${PROJECT_NAME}_HDR})

target_include_directories(${PROJECT_NAME} PRIVATE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/src;${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/uSockets/src>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/uSockets/uSockets.a -lz)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
add_dependencies(${PROJECT_NAME} uWebSockets)

add_executable(test test/meshcat_test.cpp)
target_link_libraries(test ${PROJECT_NAME})

##### Find dependencies
# find_package(CPPZMQ REQUIRED)
# find_package(nlohmann_json 3.2.0 REQUIRED)
