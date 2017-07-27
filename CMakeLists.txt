cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

project(lib-fpack C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_FLAGS "-Wall -O3 -std=c99")

include_directories(
    "${CMAKE_SOURCE_DIR}/src"
)

add_executable(example example/example.c src/fpack.c)
