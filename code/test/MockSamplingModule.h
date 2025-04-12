#pragma once

#include <string>
#include <vector>
#include <sndfile.h>

/**
 * @class MockSamplingModule
 * @brief A simple WAV file reader that provides sample-by-sample access to mono audio.
 *
 * This class loads a WAV file into memory and exposes an interface for retrieving audio samples
 * one at a time (downmixed to mono if needed). It is primarily intended for offline or test scenarios.
 */
class MockSamplingModule {
public:
    /**
     * @brief Constructs the MockSamplingModule and loads the WAV file.
     * @param filePath Path to the WAV file to load.
     */
    MockSamplingModule(const std::string& filePath);

    /**
     * @brief Retrieves the next audio sample from the loaded WAV data.
     * @param outSample Reference to a float where the sample value will be written.
     * @return True if a sample was written, false if the end of the file was reached.
     */
    bool getSample(float& outSample);

private:
    /**
     * @brief Loads the WAV file into memory and stores samples internally.
     */
    void loadWavFile();

    std::vector<float> samples;   ///< Interleaved sample buffer
    std::size_t sampleIndex = 0; ///< Index of the next sample to output
    int numChannels = 0;         ///< Number of channels in the original WAV file
};
