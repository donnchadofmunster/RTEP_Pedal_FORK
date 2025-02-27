add_library(mock_sampler MockSamplingModule.cpp)

# Ensure the library is linked correctly
target_include_directories(mock_sampler PRIVATE ${ALSA_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS})
target_link_libraries(mock_sampler PRIVATE ${ALSA_LIBRARIES} ${SNDFILE_LIBRARIES} pthread)

# Define `harmoniser_pedal` executable
add_executable(harmoniser_pedal main.cpp)
target_link_libraries(harmoniser_pedal PRIVATE mock_sampler)

# Define `main` executable (if needed separately)
add_executable(main main.cpp)
target_link_libraries(main PRIVATE mock_sampler)
