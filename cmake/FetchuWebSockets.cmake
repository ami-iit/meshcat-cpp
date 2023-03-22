# Download uSockets and uWebSockets
include(FetchContent)

FetchContent_Declare(usockets
                     URL https://github.com/uNetworking/uSockets/archive/refs/tags/v0.8.5.zip)
FetchContent_GetProperties(usockets)

FetchContent_Declare(uwebsockets
                     URL https://github.com/uNetworking/uWebSockets/archive/refs/tags/v20.37.0.zip)
FetchContent_GetProperties(uwebsockets)

if(NOT usockets_POPULATED AND NOT uwebsockets_POPULATED)
    FetchContent_Populate(usockets)
    FetchContent_Populate(uwebsockets)

    # Build uSockets
    find_package(OpenSSL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(UV REQUIRED IMPORTED_TARGET libuv)

    # See https://github.com/uNetworking/uSockets/blob/v0.8.5/Makefile#L66
    # and https://github.com/uNetworking/uSockets/blob/v0.8.5/Makefile#L74
    file(GLOB US_SRCS ${usockets_SOURCE_DIR}/src/*.c ${usockets_SOURCE_DIR}/src/eventing/*.c
                      ${usockets_SOURCE_DIR}/src/crypto/*.c ${usockets_SOURCE_DIR}/src/crypto/*.cpp)

    # Hardcode static library as we do not install the library and we do compile as shared
    add_library(uSockets STATIC ${US_SRCS})
    add_library(uSockets::uSockets ALIAS uSockets)
    # Necessary to link a static library to a shared library
    set_property(TARGET uSockets PROPERTY POSITION_INDEPENDENT_CODE ON)

    target_include_directories(uSockets PUBLIC
      $<BUILD_INTERFACE:${usockets_SOURCE_DIR}/src>
    )

    target_link_libraries(uSockets PRIVATE OpenSSL::SSL OpenSSL::Crypto PkgConfig::UV)

    # See https://github.com/uNetworking/uSockets/blob/v0.8.5/Makefile#L32
    # and https://github.com/uNetworking/uSockets/blob/v0.8.5/Makefile#L15
    target_compile_definitions(uSockets PRIVATE -DLIBUS_USE_LIBUV -DLIBUS_USE_OPENSSL)
    target_compile_features(uSockets PRIVATE cxx_std_17)

    # Define target for uWebSockets
    add_library(uWebSockets INTERFACE)
    add_library(uWebSockets::uWebSockets ALIAS uWebSockets)

    target_include_directories(uWebSockets INTERFACE
      $<BUILD_INTERFACE:${uwebsockets_SOURCE_DIR}/src>
    )

    target_link_libraries(uWebSockets INTERFACE uSockets::uSockets ZLIB::ZLIB)
    target_compile_features(uWebSockets INTERFACE cxx_std_17)
endif()

