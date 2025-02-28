#include "MockOutputModule.h"

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

void MockOutputModule::writeSamples(const std::vector<float> &samples, int numChannels)
{
    this->numChannels = numChannels;
    outputBuffer = samples;
    for (const auto &callback : callbacks)
    {
        callback(outputBuffer, numChannels);
    }
}

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