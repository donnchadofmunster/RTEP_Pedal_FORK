#include "samplingmodule.h"
#include <iostream>

SamplingModule::SamplingModule(unsigned int sampleRate, unsigned int bufferSize)
    : sampleRate(sampleRate), bufferSize(bufferSize), running(false), handle(nullptr)
{
    snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_set_params(handle, SND_PCM_FORMAT_FLOAT_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                       1, sampleRate, 1, 500000);
}

SamplingModule::~SamplingModule()
{
    stop();
    if (handle)
    {
        snd_pcm_close(handle);
    }
}

void SamplingModule::registerCallback(SampleCallback callback)
{
    callbacks.push_back(callback);
}

void SamplingModule::start()
{
    running = true;
    audioThread = std::thread(&SamplingModule::captureAudio, this);
}

void SamplingModule::stop()
{
    running = false;
    if (audioThread.joinable())
    {
        audioThread.join();
    }
}

void SamplingModule::captureAudio()
{
    std::vector<float> buffer(bufferSize);
    while (running)
    {
        int err = snd_pcm_readi(handle, buffer.data(), bufferSize);
        if (err < 0)
        {
            snd_pcm_recover(handle, err, 0);
            continue;
        }
        for (const auto &callback : callbacks)
        {
            callback(buffer);
        }
    }
}
