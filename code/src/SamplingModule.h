#ifndef SAMPLINGMODULE_H
#define SAMPLINGMODULE_H

#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <alsa/asoundlib.h>

class SamplingModule
{
public:
    using SampleCallback = std::function<void(const std::vector<float> &)>;
    SamplingModule(unsigned int sampleRate = 44100, unsigned int bufferSize = 1024);
    ~SamplingModule();
    void start();
    void stop();
    void registerCallback(SampleCallback callback);

private:
    void captureAudio();
    std::thread audioThread;
    std::atomic<bool> running;
    snd_pcm_t *handle;
    unsigned int sampleRate;
    unsigned int bufferSize;
    std::vector<SampleCallback> callbacks;
};

#endif // SAMPLINGMODULE_H