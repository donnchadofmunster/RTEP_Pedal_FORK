cmake_minimum_required(VERSION 3.7.0)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -pedantic -Werror)

project(harmoniser_pedal VERSION 1.0 LANGUAGES CXX)

# Find ALSA and libsndfile packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)

# Try finding libsndfile with pkg-config first
pkg_check_modules(SNDFILE QUIET sndfile)

if(NOT SNDFILE_FOUND)
    # If pkg-config fails, try finding it manually
    find_library(SNDFILE_LIBRARY NAMES sndfile)
    find_path(SNDFILE_INCLUDE_DIR NAMES sndfile.h PATH_SUFFIXES sndfile)
    
    if(SNDFILE_LIBRARY AND SNDFILE_INCLUDE_DIR)
        set(SNDFILE_FOUND TRUE)
        set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
        set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "libsndfile not found. Install it and ensure pkg-config can find it.")
    endif()
endif()

add_subdirectory(src)

# Add an executable target
add_executable(harmoniser_pedal src/main.cpp)

# Include ALSA and libsndfile libraries
target_include_directories(harmoniser_pedal PRIVATE ${ALSA_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS})
target_link_libraries(harmoniser_pedal PRIVATE ${ALSA_LIBRARIES} ${SNDFILE_LIBRARIES} pthread)
