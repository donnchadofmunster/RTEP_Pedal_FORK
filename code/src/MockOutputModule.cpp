#include "MockOutputModule.h"

/**
 * @brief Constructor for the MockOutputModule class.
 *
 * Initializes a new instance of MockOutputModule with default values.
 * Sets the running state to false and the number of channels to 0.
 */
MockOutputModule::MockOutputModule() : running(false), numChannels(0) {}

MockOutputModule::~MockOutputModule()
{
    stop();
}

void MockOutputModule::registerCallback(OutputCallback callback)
{
    callbacks.push_back(callback);
}

void MockOutputModule::start()
{
    running = true;
    outputThread = std::thread(&MockOutputModule::processOutput, this);
}

void MockOutputModule::stop()
{
    running = false;
    if (outputThread.joinable())
    {
        outputThread.join();
    }
}

/**
 * @brief Writes audio samples to the output buffer and triggers registered callbacks.
 *
 * This function updates the internal output buffer with new samples and notifies
 * all registered callbacks about the new data.
 *
 * @param samples A vector of float values representing the audio samples to be written.
 * @param numChannels The number of audio channels in the sample data.
 */
void MockOutputModule::writeSamples(const std::vector<float> &samples, int numChannels)
{
    this->numChannels = numChannels;
    outputBuffer = samples;
    for (const auto &callback : callbacks)
    {
        callback(outputBuffer, numChannels);
    }
}

/**
 * @brief Processes and outputs audio samples from the output buffer.
 *
 * This function runs in a separate thread and continuously checks the output buffer
 * for new audio samples. If samples are available, it prints information about the
 * number of frames and channels being output. The function sleeps for a short duration
 * between iterations to prevent excessive CPU usage.
 *
 * The function continues to run until the 'running' flag is set to false.
 *
 * @note This function is intended to be run in a separate thread.
 */
void MockOutputModule::processOutput()
{
    while (running)
    {
        if (!outputBuffer.empty())
        {
            std::cout << "Outputting " << outputBuffer.size() / numChannels << " frames with " << numChannels << " channels.\r" << std::flush;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}