# Collect all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")

# Define the main pedal library using all source files (except main.cpp)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
add_library(pedal_lib ${SOURCES})

# Include current source directory for headers
target_include_directories(pedal_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link dependencies
target_link_libraries(pedal_lib PRIVATE pthread PRIVATE sndfile)

# Define the executable and link the library
add_executable(harmoniser_pedal main.cpp)
target_link_libraries(harmoniser_pedal PRIVATE pedal_lib stdc++fs)
