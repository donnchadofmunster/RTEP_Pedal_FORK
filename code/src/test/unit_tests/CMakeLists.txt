# Locate GoogleTest (requires GTest to be installed or fetched via FetchContent)
find_package(GTest REQUIRED)

# Define the unit test executable
add_executable(unit_tests unit_tests.cpp)

# Include directories for test and production code
target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/test
    ${CMAKE_SOURCE_DIR}/src/effects
)

# Reuse mock modules
target_sources(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/test/MockSamplingModule.cpp
    ${CMAKE_SOURCE_DIR}/src/test/MockOutputModule.cpp
)

# Link test executable with GTest and your library
target_link_libraries(unit_tests PRIVATE
    pedal_lib
    GTest::gtest
    GTest::gtest_main
)

# Enable automatic discovery of tests
include(GoogleTest)
gtest_discover_tests(unit_tests)

# Optional: group under "Tests" in IDEs like CLion/VSCode
# set_target_properties(unit_tests PROPERTIES FOLDER "Tests")
