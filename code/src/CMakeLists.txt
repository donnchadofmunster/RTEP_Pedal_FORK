# Collect all .cpp files in the current source directory
file(GLOB SRC_SOURCES "*.cpp")

# Collect all .cpp files in the effects directory
file(GLOB EFFECT_SOURCES "${CMAKE_SOURCE_DIR}/effects/*.cpp")

# Combine both sets of sources
set(ALL_SOURCES ${SRC_SOURCES} ${EFFECT_SOURCES})

# Remove main.cpp from the library sources
list(REMOVE_ITEM ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Define the main pedal library
add_library(pedal_lib ${ALL_SOURCES})

# Include directories for source and effects
target_include_directories(pedal_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/effects
)

# Link required libraries
target_link_libraries(pedal_lib PRIVATE pthread sndfile)

# Define the executable and link the pedal library
add_executable(harmoniser_pedal main.cpp)
target_link_libraries(harmoniser_pedal PRIVATE pedal_lib stdc++fs)
