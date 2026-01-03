cmake_minimum_required(VERSION 3.31)

project(drwlm VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS ON)

add_compile_definitions(_XOPEN_SOURCE _GNU_SOURCE)

add_subdirectory(cli)
add_subdirectory(common)
add_subdirectory(daemon)
add_subdirectory(lib)
