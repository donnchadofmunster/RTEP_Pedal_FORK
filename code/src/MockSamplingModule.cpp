#include "MockSamplingModule.h"
#include <iostream>

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

    samples.resize(sfinfo.frames * sfinfo.channels); // Resize to accommodate all channels
    sf_read_float(file, samples.data(), sfinfo.frames * sfinfo.channels);
    sf_close(file);

    numChannels = sfinfo.channels; // Store the number of channels
}

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
