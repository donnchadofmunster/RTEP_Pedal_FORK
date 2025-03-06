#ifndef MOCKOUTPUTMODULE_H
#define MOCKOUTPUTMODULE_H

#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <sndfile.h>
#include "Sample.h"

/**
 * @class MockOutputModule
 * @brief Simulates an audio output module by writing Sample objects to a WAV file.
 */
class MockOutputModule
{
public:
    using OutputCallback = std::function<void(const Sample &)>;

    /**
     * @brief Constructor for MockOutputModule.
     * @param outputFilePath Path to the output WAV file.
     */
    MockOutputModule(const std::string &outputFilePath);

    /**
     * @brief Destructor for MockOutputModule.
     * Ensures that output processing is stopped and the WAV file is written.
     */
    ~MockOutputModule();

    /**
     * @brief Starts the output processing thread.
     */
    void start();

    /**
     * @brief Stops the output processing and finalizes the WAV file.
     */
    void stop();

    /**
     * @brief Registers a callback to be invoked when new samples are written.
     * @param callback A function that takes Sample objects.
     */
    void registerCallback(OutputCallback callback);

    /**
     * @brief Writes a Sample object to the output buffer and notifies callbacks.
     * @param sample The Sample object containing PCM data and metadata.
     */
    void writeSample(const Sample &sample);

private:
    /**
     * @brief Continuously processes output samples in a separate thread.
     */
    void processOutput();

    /**
     * @brief Writes buffered audio samples to a WAV file.
     */
    void writeWavFile();

    std::vector<OutputCallback> callbacks; ///< List of registered callbacks.
    std::vector<Sample> outputBuffer;      ///< Buffer holding the output samples.
    int numChannels;                       ///< Number of audio channels.
    std::atomic<bool> running;             ///< Flag indicating whether output is running.
    std::thread outputThread;              ///< Thread handling output processing.
    std::string outputFilePath;            ///< Path of the output WAV file.
};

#endif // MOCKOUTPUTMODULE_H