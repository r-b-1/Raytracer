cmake_minimum_required(VERSION 3.22)

project(cs4212StarterCode
  VERSION 1.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

if(ENABLE_CPPCHECK)
  find_program(CPPCHECK cppcheck)
  if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK}
        --suppress=missingInclude
        --enable=all
        --inline-suppr
        --inconclusive
        -i
        ${CMAKE_SOURCE_DIR})
  else()
    message(SEND_ERROR "cppcheck requested but executable not found")
  endif()
endif()

if(ENABLE_CLANG_TIDY)
  find_program(CLANGTIDY clang-tidy-13)
  if(CLANGTIDY)
    set(CMAKE_C_CLANG_TIDY ${CLANGTIDY})
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY})
  else()
    message(SEND_ERROR "clang-tidy could not be located.")
  endif()
endif()

# Require standard C++20 for all project targets, without compiler extensions.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------------------------
# PNG and ZLIB
#
# This macro will attempt to locate the PNG include directories, link directories and libraries. The INCLUDE_DIRECTORIES
# macro adds the resulting include directories to the search path.
# --------------------------------------------------------------------
FIND_PACKAGE(PNG REQUIRED)
if(NOT PNG_FOUND)
  message(STATUS "Could not find the PNG Libraries!")
endif(NOT PNG_FOUND)

Include(FetchContent)

# Catch2
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)

# Boost
# Necessary to get Win32 version to work, plus, not a bad idea to link statically for our purposes.
if(WIN32)
  set(Boost_USE_STATIC_LIBS ON)
endif(WIN32)

find_package(Boost REQUIRED COMPONENTS program_options)
if(NOT ${Boost_FOUND})
  message(STATUS "Could not find the Boost Libraries!")
endif()

# --------------------------------------------------------------------
# OpenGL Related Components
# --------------------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# not using Vulkan at the moment
# find_package(Vulkan)

# --------------------------------------------------------------------
# OpenMP
#
# AppleClang rejects -fopenmp outright; the OpenMP runtime has to come from
# Homebrew's libomp and -fopenmp has to go through the preprocessor. Point
# FindOpenMP at that runtime so it can detect OpenMP instead of failing.
# --------------------------------------------------------------------
if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  find_library(
    OpenMP_omp_LIBRARY
    NAMES omp
    HINTS /opt/homebrew/opt/libomp/lib /usr/local/opt/libomp/lib)
  find_path(
    OpenMP_CXX_INCLUDE_DIR
    NAMES omp.h
    HINTS /opt/homebrew/opt/libomp/include /usr/local/opt/libomp/include)
  if(OpenMP_omp_LIBRARY)
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${OpenMP_CXX_INCLUDE_DIR}")
    set(OpenMP_CXX_LIB_NAMES omp)
    set(OpenMP_CXX_LIBRARIES ${OpenMP_omp_LIBRARY})
  endif()
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  message(STATUS "OpenMP enabled: ${OpenMP_CXX_FLAGS}")
else()
  message(STATUS "OpenMP not available - rendering will be single-threaded.")
endif()

# Add the src directory to build the library.
add_subdirectory(src)

# Add my renderlib
#add_subdirectory(renderlib)

# Some examples
add_subdirectory(examples)

#
# Renderer code
#
# Create your rendering library here....


#
# OpenGL Example
#
# This builds a simple OpenGL example that you can use to get started with OpenGL.
#
add_subdirectory(OpenGL)
# add_executable(OpenGL
#   OpenGL/glfwExample.cpp
# )
# add_subdirectory(Vulkan)

#
# Unit Tests - by enabling testing with cmake, we can create a target that lets unit tests be run. After re-building
# yourproject with ENABLE_TESTING called, a make target, called test will be enabled. You can then type
#
# make test
#
enable_testing()
add_subdirectory(utests)
add_executable(main 
  src/main.cc)
target_link_libraries(main PRIVATE cs4212-util PNG::PNG)
