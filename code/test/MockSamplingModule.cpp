#include "MockSamplingModule.h"
#include <iostream>

/**
 * @brief Constructor for the MockSamplingModule class.
 *
 * Initializes a new MockSamplingModule instance with the specified WAV file path.
 * The constructor sets up the initial state and reads the WAV file.
 *
 * @param wavFilePath The file path of the WAV file to be read and processed.
 */
MockSamplingModule::MockSamplingModule(const std::string &wavFilePath)
    : wavFilePath(wavFilePath), running(false)
{
    readWavFile();
}

/**
 * @brief Destructor for the MockSamplingModule class.
 *
 * This destructor ensures that any ongoing audio processing is stopped
 * before the object is destroyed. It calls the stop() method to halt
 * any running audio threads and perform necessary cleanup.
 */
MockSamplingModule::~MockSamplingModule()
{
    stop();
}

/**
 * @brief Registers a callback function to be invoked during audio processing.
 *
 * This function adds the provided callback to the list of callbacks that will be
 * called when audio samples are processed. Multiple callbacks can be registered.
 *
 * @param callback A function pointer or callable object of type SampleCallback.
 *                 This callback will be invoked with processed audio samples.
 *
 * @note The registered callbacks are typically called in the order they were added.
 */
void MockSamplingModule::registerCallback(SampleCallback callback)
{
    callbacks.push_back(callback);
}

/**
 * @brief Starts the audio processing thread.
 *
 * This function initiates the audio processing by setting the running flag to true
 * and launching a new thread that executes the processAudio method. Once called,
 * the MockSamplingModule begins processing audio samples and invoking registered callbacks.
 *
 * @note This function is non-blocking. The audio processing occurs in a separate thread.
 */
void MockSamplingModule::start()
{
    running = true;
    audioThread = std::thread(&MockSamplingModule::processAudio, this);
}

/**
 * @brief Stops the audio processing thread.
 *
 * This function halts the audio processing by setting the running flag to false
 * and waiting for the audio processing thread to finish its execution. It ensures
 * that the audio thread is properly terminated before the function returns.
 *
 * @note This function is blocking and will wait for the audio thread to complete
 *       before returning.
 */
void MockSamplingModule::stop()
{
    running = false;
    if (audioThread.joinable())
    {
        audioThread.join();
    }
}

/**
 * @brief Reads and processes a WAV file.
 *
 * This function opens the WAV file specified by wavFilePath, reads its contents,
 * and stores the audio samples in the samples vector. It also sets the number
 * of channels based on the WAV file's properties.
 *
 * @throws None, but prints an error message to cerr if the file cannot be opened.
 *
 * @note This function uses the libsndfile library to read the WAV file.
 */
void MockSamplingModule::readWavFile()
{
    SNDFILE *file;
    SF_INFO sfinfo;

    file = sf_open(wavFilePath.c_str(), SFM_READ, &sfinfo);
    if (!file)
    {
        std::cerr << "Error opening WAV file: " << wavFilePath << std::endl;
        return;
    }

    samples.resize(sfinfo.frames * sfinfo.channels); // Resize to accommodate all channels
    sf_read_float(file, samples.data(), sfinfo.frames * sfinfo.channels);
    sf_close(file);

    numChannels = sfinfo.channels; // Store the number of channels
}

/**
 * @brief Processes audio samples and invokes registered callbacks.
 *
 * This function separates the interleaved audio samples into individual channel vectors,
 * then invokes all registered callbacks with the original interleaved sample data.
 *
 * @note This function is typically called in a separate thread initiated by the start() method.
 *
 * @see start()
 * @see registerCallback()
 */
void MockSamplingModule::processAudio()
{
    std::vector<std::vector<float>> channelData(numChannels);

    // Separate samples into different channel vectors
    for (size_t i = 0; i < samples.size(); i += numChannels)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            channelData[ch].push_back(samples[i + ch]);
        }
    }

    // Invoke callbacks for each channel
    for (const auto &callback : callbacks)
    {
        callback(samples); // Send all channels interleaved
    }
}

int MockSamplingModule::getNumChannels() const { return numChannels; }
