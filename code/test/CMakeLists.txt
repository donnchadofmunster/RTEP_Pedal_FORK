# Collect all effect source files from /effects
file(GLOB EFFECT_SOURCES "${CMAKE_SOURCE_DIR}/effects/*.cpp")

# Define the test helper library
add_library(test_lib
    MockSamplingModule.cpp
    MockOutputModule.cpp
)

# Disable signalsmith memory tracker for test builds
target_compile_definitions(test_lib PRIVATE DISABLE_MEMORY_TRACKER)

# Include headers for test_lib
target_include_directories(test_lib PRIVATE
    ${ALSA_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

# Link core libraries to test_lib
target_link_libraries(test_lib PRIVATE
    ${ALSA_LIBRARIES}
    ${SNDFILE_LIBRARIES}
    pthread
)

# Define the test executable, pulling in effect sources and ForceEffects from src
add_executable(test
    test.cpp
    ${CMAKE_SOURCE_DIR}/effects/ForceEffects.cpp  # explicitly include from /src
    ${EFFECT_SOURCES}                          # drag-and-drop friendly
)

# Include headers and link everything needed for test
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test PRIVATE test_lib pedal_lib)
