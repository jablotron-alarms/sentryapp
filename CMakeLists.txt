cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# create project
project(testapp)

# we require c++17 for this app
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "RelWithDebInfo" "Release"
                                               "MinSizeRel" "Debug")
endif()

option(ENABLE_CCACHE "Enable caching of compilation invocation with ccache" TRUE)
if(ENABLE_CCACHE)
  find_program(CCACHE ccache)
  if(CCACHE)
    message("using ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  else()
    message("ccache not found cannot use")
  endif()
endif()

# add dependencies
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME CLI11
    # the git repo takes quite a long time to clone, so we download the packaged version instead
    URL https://github.com/CLIUtils/CLI11/archive/v1.8.0.zip
    URL_HASH SHA256=fab4d010d5c3b0886e65f0dbe28ebccc9164e5b4d1a22ac1dc4c290cfd51d0aa
    VERSION 1.8.0
)

CPMAddPackage(
    NAME sentry
    GITHUB_REPOSITORY getsentry/sentry-native
    GIT_TAG 0.4.4
    OPTIONS
        "SENTRY_BACKEND breakpad"
)

# add executable
add_executable(testapp main.cpp)

# link dependencies
target_link_libraries(testapp CLI11 sentry)