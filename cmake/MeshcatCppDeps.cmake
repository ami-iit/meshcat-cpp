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


option(MESHCAT_CPP_USE_SYSTEM_MSGPACK "Use system msgpack" OFF)
mark_as_advanced(MESHCAT_CPP_USE_SYSTEM_MSGPACK)
if(MESHCAT_CPP_USE_SYSTEM_MSGPACK)
  find_package(msgpack REQUIRED)
else()
  include(FetchContent)
  FetchContent_Declare(
    msgpack
    GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
    GIT_TAG        cpp-4.1.3
  )
  FetchContent_MakeAvailable(msgpack)
endif()


find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

