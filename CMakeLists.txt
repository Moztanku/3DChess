cmake_minimum_required(VERSION 3.29)

project(3DChess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Build type (Debug, Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")

set(DEBUG_FLAGS "-g -O0 -DDEBUG -Wno-unused-parameter")
set(RELEASE_FLAGS "-Ofast -flto -DNDEBUG")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}")
    # Build shared libraries to reduce linking time
    set(BUILD_SHARED_LIBS ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS}")
    # Build static libraries to improve performance
    set(BUILD_SHARED_LIBS OFF)
else()
    message(FATAL_ERROR "Invalid build type")
endif()

## Directories
set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/inc)

## External dependencies
# - GLAD            (OpenGL loader)
# - glm             (Mathematics)
# - glfw3           (Window and input)
# - STB_image       (Image loading)
# - Assimp          (3D model loading)
# - JacekLib        (My utility library)
add_subdirectory(${VENDOR_DIR})

## OpenGL
find_package(OpenGL REQUIRED)

if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found. Please install it using this guide `https://www.khronos.org/opengl/wiki/Getting_Started#Downloading_OpenGL`.")
endif()

## Sources
file(GLOB_RECURSE SOURCES ${SRC_DIR}/**.cpp)

## Executable
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE
    STB_image
    glfw
    GLAD
    glm
    GL
    assimp
    JacekLib)
