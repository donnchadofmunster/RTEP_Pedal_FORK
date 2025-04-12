#pragma once

#include <string>
#include <vector>
#include "Sample.h"
#include <sndfile.h>

/**
 * @class MockOutputModule
 * @brief A mock audio output module that collects processed samples and writes them to a WAV file.
 *
 * This class buffers samples written to it and writes them to disk as a single-channel 44.1 kHz
 * floating-point WAV file when `saveToFile()` is called. It is useful for offline testing of DSP chains.
 */
class MockOutputModule {
public:
    /**
     * @brief Constructs the output module with the target file path.
     * @param outputPath Path to write the final WAV file to.
     */
    MockOutputModule(const std::string& outputPath);

    /**
     * @brief Buffers a single audio sample.
     * @param sample The processed sample to store.
     */
    void writeSample(const Sample& sample);

    /**
     * @brief Writes all buffered samples to a WAV file.
     *
     * The output will be saved as a mono, 44.1 kHz, 32-bit float WAV file.
     */
    void saveToFile();

private:
    std::vector<float> pcmSamples; ///< Buffer of raw PCM sample values
    std::string outputPath;        ///< Destination file path for output WAV
};
