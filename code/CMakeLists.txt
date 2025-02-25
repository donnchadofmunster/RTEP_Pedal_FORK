cmake_minimum_required(VERSION 3.7.0)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -pedantic -Werror)

project(harmoniser_pedal VERSION 1.0 LANGUAGES CXX)

add_subdirectory(src)