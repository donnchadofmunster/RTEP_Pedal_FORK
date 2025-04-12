#ifndef SAMPLINGMODULE_H
#define SAMPLINGMODULE_H

#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <alsa/asoundlib.h>

// SamplingModule class handles audio input capture from a CODEC device
class SamplingModule
{
public:
    using SampleCallback = std::function<void(const std::vector<float> &)>;

    // Constructor initializes the sampling rate and buffer size
    SamplingModule(unsigned int sampleRate = 44100, unsigned int bufferSize = 1024);
    ~SamplingModule();

    // Starts the audio capturing process in a separate thread
    void start();

    // Stops the audio capturing process
    void stop();

    // Registers a callback function to process captured samples
    void registerCallback(SampleCallback callback);

private:
    // Function executed by the audio thread to capture and process samples
    void captureAudio();

    std::thread audioThread;               // Thread for audio processing

    unsigned int sampleRate;               // Sample rate in Hz
    unsigned int bufferSize;               // Buffer size for capturing audio
    std::atomic<bool> running;             // Flag to indicate if capturing is running
    snd_pcm_t *handle;                     // ALSA PCM handle for audio input
    std::vector<SampleCallback> callbacks; // List of registered callbacks
};

#endif // SAMPLINGMODULE_H