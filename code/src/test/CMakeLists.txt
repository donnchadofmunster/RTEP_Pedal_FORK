# src/test/CMakeLists.txt

# --- Define a lightweight helper library for test-only components ---
# This includes mocks like the fake input/output modules used in testing

add_library(test_lib
    MockSamplingModule.cpp
    MockOutputModule.cpp
)

# Disable the Signalsmith memory tracker for clean valgrind output in tests
target_compile_definitions(test_lib PRIVATE DISABLE_MEMORY_TRACKER)

# --- Include paths for test_lib ---
# This allows test_lib to include any module from src/
# Including the directory where Sample.h lives: src/sampling

target_include_directories(test_lib PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/display
    ${CMAKE_SOURCE_DIR}/src/dsp
    ${CMAKE_SOURCE_DIR}/src/sampling
    ${CMAKE_SOURCE_DIR}/src/effects
    ${ALSA_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
)

# --- Link test_lib with system dependencies ---
target_link_libraries(test_lib PRIVATE
    ${ALSA_LIBRARIES}
    ${SNDFILE_LIBRARIES}
    pthread
)

# --- Define the main functional test binary ---
# This runs the real-time test pipeline (e.g., loading effects, processing samples)

add_executable(func_test
    test.cpp
)

# --- Include full source headers for func_test ---
# This includes everything from the pedal project
target_include_directories(func_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# --- Link func_test with the pedal runtime and test support library ---
target_link_libraries(func_test PRIVATE
    test_lib
    pedal_lib
)

# --- Add the unit test directory if you use GoogleTest or similar ---
# This builds unit test binaries and integrates with CTest

add_subdirectory(unit_tests)
