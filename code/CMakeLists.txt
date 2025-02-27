cmake_minimum_required(VERSION 3.7.0)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -pedantic -Werror)

project(harmoniser_pedal VERSION 1.0 LANGUAGES CXX)

# Find ALSA package
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)

add_subdirectory(src)

# Add an executable target
add_executable(harmoniser_pedal src/main.cpp)  # Ensure main.cpp exists

# Include ALSA library and directories
target_include_directories(harmoniser_pedal PRIVATE ${ALSA_INCLUDE_DIRS})
target_link_libraries(harmoniser_pedal PRIVATE ${ALSA_LIBRARIES} pthread)
