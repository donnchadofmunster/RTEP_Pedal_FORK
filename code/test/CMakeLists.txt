# Define the testing library
add_library(testing_lib MockSamplingModule.cpp MockOutputModule.cpp)

# Ensure the library is linked correctly
target_include_directories(testing_lib PRIVATE ${ALSA_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(testing_lib PRIVATE ${ALSA_LIBRARIES} ${SNDFILE_LIBRARIES} pthread)

# Define `test` executable for testing
add_executable(test test.cpp)

# Ensure `test` has access to DigitalSignalChain from `pedal_lib`
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test PRIVATE testing_lib pedal_lib)
