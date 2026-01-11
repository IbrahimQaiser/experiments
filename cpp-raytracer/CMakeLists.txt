cmake_minimum_required(VERSION 3.10)
project(raytracer)
set(CMAKE_CXX_STANDARD 11)
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)
if (MSVC)
    add_compile_options(/W4)
else()
    # add_compile_options(-Wall -Wextra -pedantic -O0 -ggdb3 -Wno-missing-field-initializers -Wno-unused-parameter)
    add_compile_options(-Wall -Wextra -pedantic -O3 -Wno-missing-field-initializers -Wno-unused-parameter)
endif()

add_executable(raytracer src/main.cpp src/camera.cpp src/image.cpp src/scene.cpp src/objects.cpp src/material.cpp src/raytracer.cpp)
