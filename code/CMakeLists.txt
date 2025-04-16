cmake_minimum_required(VERSION 3.7.0)

project(shred_pedal VERSION 1.0 LANGUAGES CXX)

# Use C++17 and enable warnings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable optimisations and LTO
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
add_compile_options(-Wall -pedantic -Werror)

# Find ALSA using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)

# Try finding libsndfile with pkg-config first
pkg_check_modules(SNDFILE QUIET sndfile)

# Fallback if pkg-config failed
if(NOT SNDFILE_FOUND)
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

# Export ALSA and SNDFILE settings to subdirectories (like src/)
set(ALSA_INCLUDE_DIRS ${ALSA_INCLUDE_DIRS} PARENT_SCOPE)
set(ALSA_LIBRARIES ${ALSA_LIBRARIES} PARENT_SCOPE)
set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIRS} PARENT_SCOPE)
set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARIES} PARENT_SCOPE)

# Enable test discovery
enable_testing()

# Add the source tree
add_subdirectory(src)
