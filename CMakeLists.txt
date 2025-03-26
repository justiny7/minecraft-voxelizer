cmake_minimum_required(VERSION 3.10)
project(MinecraftReconstructor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
    src/main.cpp
    src/voxelgrid.cpp
    src/bbox.cpp
    src/camera.cpp
    src/rand.cpp
    src/voxelizer.cpp
    src/image.cpp
    src/parser.cpp
    src/light.cpp
    src/bsdf.cpp
)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost 1.84.0 REQUIRED COMPONENTS program_options)
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

add_executable(MinecraftReconstructor ${SOURCES})

target_include_directories(MinecraftReconstructor PRIVATE include src)
target_link_libraries(MinecraftReconstructor PRIVATE
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
)

target_compile_options(MinecraftReconstructor PRIVATE -Ofast)
