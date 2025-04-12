# src/test/CMakeLists.txt

# Collect all effect sources to ensure drag-and-drop effects are testable
file(GLOB EFFECT_SOURCES "${CMAKE_SOURCE_DIR}/src/effects/*.cpp")

# Define the helper library for functional testing
add_library(test_lib
    MockSamplingModule.cpp
    MockOutputModule.cpp
)

# Disable Signalsmith memory tracker for test builds
target_compile_definitions(test_lib PRIVATE DISABLE_MEMORY_TRACKER)

# Include directories for test_lib
target_include_directories(test_lib PRIVATE
    ${ALSA_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/display
    ${CMAKE_SOURCE_DIR}/src/dsp
    ${CMAKE_SOURCE_DIR}/src/sampling
    ${CMAKE_SOURCE_DIR}/src/effects
)

# Link external libs to test_lib
target_link_libraries(test_lib PRIVATE
    ${ALSA_LIBRARIES}
    ${SNDFILE_LIBRARIES}
    pthread
)

# Define the functional test executable
add_executable(func_test
    test.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp/ForceEffects.cpp  # ensure auto-registration
    ${EFFECT_SOURCES}
)

# Include headers for func_test and link libraries
target_include_directories(func_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(func_test PRIVATE test_lib pedal_lib)

# Enable test discovery and add unit tests directory

add_subdirectory(unit_tests)
