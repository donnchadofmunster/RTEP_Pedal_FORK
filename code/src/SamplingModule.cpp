#include "SamplingModule.h"
#include <iostream>

// Constructor: Initializes the ALSA PCM device with the specified parameters
SamplingModule::SamplingModule(unsigned int sampleRate, unsigned int bufferSize)
    : sampleRate(sampleRate), bufferSize(bufferSize), running(false), handle(nullptr)
{
    // Open the default audio capture device
    snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);

    // Configure PCM parameters: format, access type, channels, sample rate, and latency
    snd_pcm_set_params(handle, SND_PCM_FORMAT_FLOAT_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                       1, sampleRate, 1, 500000);
}

// Destructor: Ensures proper cleanup of resources
SamplingModule::~SamplingModule()
{
    stop(); // Stop capturing if still running
    if (handle)
    {
        snd_pcm_close(handle); // Close PCM device
    }
}

// Register a callback function to process captured samples
void SamplingModule::registerCallback(SampleCallback callback)
{
    callbacks.push_back(callback);
}

// Start capturing audio in a separate thread
void SamplingModule::start()
{
    running = true;
    audioThread = std::thread(&SamplingModule::captureAudio, this);
}

// Stop capturing audio and join the thread
void SamplingModule::stop()
{
    running = false;
    if (audioThread.joinable())
    {
        audioThread.join();
    }
}

// Audio capture function that runs in a separate thread
void SamplingModule::captureAudio()
{
    std::vector<float> buffer(bufferSize); // Buffer to store captured samples

    while (running)
    {
        // Read audio samples from the PCM device into the buffer
        int err = snd_pcm_readi(handle, buffer.data(), bufferSize);

        if (err < 0)
        {                                    // Handle errors in audio capture
            snd_pcm_recover(handle, err, 0); // Attempt to recover from error
            continue;
        }

        // Invoke all registered callback functions with the captured samples
        for (const auto &callback : callbacks)
        {
            callback(buffer);
        }
    }
}
