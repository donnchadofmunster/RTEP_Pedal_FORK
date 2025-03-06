#include "MockSamplingModule.h"
#include "Sample.h"
#include <iostream>
#include <chrono>
#include <thread>

MockSamplingModule::MockSamplingModule(const std::string &wavFilePath)
    : wavFilePath(wavFilePath), running(false)
{
    readWavFile();
}

MockSamplingModule::~MockSamplingModule()
{
    stop();
}

void MockSamplingModule::registerCallback(SampleCallback callback)
{
    callbacks.push_back(callback);
}

void MockSamplingModule::start()
{
    running = true;
    audioThread = std::thread(&MockSamplingModule::processAudio, this);
}

void MockSamplingModule::stop()
{
    running = false;
    if (audioThread.joinable())
    {
        audioThread.join();
    }
}

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

    samples.resize(sfinfo.frames * sfinfo.channels);
    sf_read_float(file, samples.data(), sfinfo.frames * sfinfo.channels);
    sf_close(file);

    numChannels = sfinfo.channels;
}

void MockSamplingModule::processAudio()
{
    const double sampleRate = 44100.0;
    const double frameDuration = 1.0 / sampleRate;

    for (size_t i = 0; i < samples.size(); i += numChannels)
    {
        if (!running)
            break;

        double timeIndex = (i / numChannels) * frameDuration;
        Sample sample(samples[i], timeIndex);

        // Invoke callbacks immediately
        for (const auto &callback : callbacks)
        {
            callback(sample);
        }

        // Simulate real-time delay
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(frameDuration * 1e6)));
    }
}

bool MockSamplingModule::isRunning() const
{
    return running.load();
}
