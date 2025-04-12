# src/test/unit_tests/CMakeLists.txt

# Locate GoogleTest (requires GTest to be installed or FetchContent-managed)
find_package(GTest REQUIRED)

# Define the unit test executable
add_executable(unit_tests unit_tests.cpp)

# Include directories for test and source code
target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/test
)

# Link libraries: GoogleTest and your pedal logic
target_link_libraries(unit_tests PRIVATE
    GTest::gtest
    GTest::gtest_main
    pedal_lib
)

# Optionally reuse the mock sampling/output modules for tests
target_sources(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/test/MockSamplingModule.cpp
    ${CMAKE_SOURCE_DIR}/src/test/MockOutputModule.cpp
)

# Enable automatic test discovery
include(GoogleTest)
gtest_discover_tests(unit_tests)
