cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# create project
project(sentryapp)

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

find_package(sentry REQUIRED)

add_executable(sentryapp example.c)
target_link_libraries(sentryapp sentry::sentry pthread)
