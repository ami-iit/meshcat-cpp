# Some dependencies are available in conda-forge but not in Debian/Ubuntu
# If we are in a conda environment, let's assume that the dependency are available
# and use them by default. Advanced user can always cahnge the MESHCAT_CPP_USE_SYSTEM_*
# variables to match the desired behaviour
if(DEFINED ENV{CONDA_PREFIX})
  set(MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS_DEFAULT ON)
  set(MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX_DEFAULT ON)
else()
  set(MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS_DEFAULT OFF)
  set(MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX_DEFAULT OFF)
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
    GIT_TAG        v1.2.3
  )
  FetchContent_GetProperties(stduuid)
  if(NOT stduuid_POPULATED)
    FetchContent_Populate(stduuid)
    #set(UUID_USING_CXX20_SPAN ON CACHE BOOL "" FORCE)
    add_subdirectory(${stduuid_SOURCE_DIR} ${stduuid_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endif()


option(MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX "Use system msgpack-cxx" ${MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX_DEFAULT})
mark_as_advanced(MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX)
if(MESHCAT_CPP_USE_SYSTEM_MSGPACKCXX)
  find_package(msgpack-cxx REQUIRED)
else()
  include(FetchContent)
  FetchContent_Declare(
    msgpack-cxx
    URL https://github.com/msgpack/msgpack-c/archive/refs/tags/cpp-6.0.0.zip
  )
  FetchContent_GetProperties(msgpack-cxx)
  if(NOT msgpack-cxx_POPULATED)
    FetchContent_Populate(msgpack-cxx)
    add_subdirectory(${msgpack-cxx_SOURCE_DIR} ${msgpack-cxx_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endif()

option(MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS "Use system uwebsockets" ${MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS_DEFAULT})
if(NOT MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS AND NOT BUILD_SHARED_LIBS)
    message(FATAL_ERROR "MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS and BUILD_SHARED_LIBS can't be OFF at the same time")
endif()
mark_as_advanced(MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS)
if(MESHCAT_CPP_USE_SYSTEM_UWEBSOCKETS)
  find_package(uWebSockets REQUIRED)
else()
  include(FetchuWebSockets)
endif()

# Handle relocatability via cmrc
option(MESHCAT_CPP_USE_SYSTEM_CMRC "Use system cmrc" OFF)
mark_as_advanced(MESHCAT_CPP_USE_SYSTEM_CMRC)
if(MESHCAT_CPP_USE_SYSTEM_CMRC)
  find_package(CMakeRC REQUIRED)
else()
  include(FetchContent)
  FetchContent_Declare(
    cmrc
    GIT_REPOSITORY https://github.com/vector-of-bool/cmrc.git
    GIT_TAG        2.0.1
  )
  FetchContent_GetProperties(cmrc)
  if(NOT cmrc_POPULATED)
    FetchContent_Populate(cmrc)
    add_subdirectory(${cmrc_SOURCE_DIR} ${cmrc_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endif()
