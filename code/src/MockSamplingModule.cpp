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

    samples.resize(sfinfo.frames);
    sf_read_float(file, samples.data(), sfinfo.frames);
    sf_close(file);
}

void MockSamplingModule::processAudio()
{
    for (const auto &callback : callbacks)
    {
        callback(samples);
    }
}
