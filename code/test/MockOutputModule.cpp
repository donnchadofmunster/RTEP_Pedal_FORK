#include "MockOutputModule.h"

MockOutputModule::MockOutputModule(const std::string &outputFilePath)
    : callbacks(),
      outputBuffer(),
      numChannels(0),
      running(false),
      outputThread(),
      outputFilePath(outputFilePath)
{
}

MockOutputModule::~MockOutputModule()
{
    stop();
    writeWavFile();
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
    writeWavFile();
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

void MockOutputModule::writeWavFile()
{
    if (outputBuffer.empty() || numChannels == 0)
        return;

    SF_INFO sfinfo;
    sfinfo.samplerate = 44100; // Default sample rate
    sfinfo.channels = numChannels;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    SNDFILE *file = sf_open(outputFilePath.c_str(), SFM_WRITE, &sfinfo);
    if (!file)
    {
        std::cerr << "Error creating WAV file: " << outputFilePath << std::endl;
        return;
    }

    sf_write_float(file, outputBuffer.data(), outputBuffer.size());
    sf_close(file);

    std::cout << "WAV file written: " << outputFilePath << std::endl;
}
