#ifndef MOCKOUTPUTMODULE_H
#define MOCKOUTPUTMODULE_H

#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <sndfile.h>

/**
 * @class MockOutputModule
 * @brief Simulates an audio output module by writing audio data to a WAV file.
 */
class MockOutputModule
{
public:
    using OutputCallback = std::function<void(const std::vector<float> &, int)>;

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
     * @param callback A function that takes sample data and the number of channels.
     */
    void registerCallback(OutputCallback callback);

    /**
     * @brief Writes audio samples to the output buffer and notifies callbacks.
     * @param samples The vector of audio samples.
     * @param numChannels Number of audio channels.
     */
    void writeSamples(const std::vector<float> &samples, int numChannels);

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
    std::vector<float> outputBuffer;       ///< Buffer holding the output samples.
    int numChannels;                       ///< Number of audio channels.
    std::atomic<bool> running;             ///< Flag indicating whether output is running.
    std::thread outputThread;              ///< Thread handling output processing.
    std::string outputFilePath;            ///< Path of the output WAV file.
};

#endif // MOCKOUTPUTMODULE_H