# Handle relocatability via reloc-cpp
option(MESHCAT_CPP_USE_SYSTEM_RELOC_CPP "Use system reloc-cpp" OFF)
mark_as_advanced(MESHCAT_CPP_USE_SYSTEM_RELOC_CPP)
if(MESHCAT_CPP_USE_SYSTEM_RELOC_CPP)
  find_package(reloc-cpp REQUIRED)
else()
  include(FetchContent)
  FetchContent_Declare(
    reloc-cpp
    GIT_REPOSITORY https://github.com/ami-iit/reloc-cpp.git
    GIT_TAG        v0.1.0
  )
  FetchContent_MakeAvailable(reloc-cpp)
endif()

option(MESHCAT_CPP_USE_SYSTEM_STDUUID "Use system stduuid" OFF)
mark_as_advanced(MESHCAT_CPP_USE_SYSTEM_STDUUID)
if(MESHCAT_CPP_USE_SYSTEM_STDUUID)
  find_package(stduuid REQUIRED)
else()
  include(FetchContent)
  FetchContent_Declare(
    stduuid
    GIT_REPOSITORY https://github.com/mariusbancila/stduuid.git
    GIT_TAG        v1.2.2
  )
  FetchContent_MakeAvailable(stduuid)
endif()


option(MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX "Use system msgpack-cxx" OFF)
mark_as_advanced(MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX)
if(MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX)
  find_package(msgpack-cxx REQUIRED)
else()
  include(FetchContent)
  FetchContent_Declare(
    msgpackcxx
    URL https://github.com/msgpack/msgpack-c/archive/refs/tags/cpp-6.0.0.zip
  )
  FetchContent_MakeAvailable(msgpackcxx)
endif()

option(MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS "Use system uwebsockets" OFF)
if(NOT MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS AND NOT BUILD_SHARED_LIBS)
    message(FATAL_ERROR "MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS and BUILD_SHARED_LIBS can't be OFF at the same time")
endif()
mark_as_advanced(MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS)
if(MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS)
  find_package(uWebSockets REQUIRED)
else()
  include(FetchuWebSockets)
endif()
