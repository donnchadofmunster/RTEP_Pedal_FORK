#ifndef MOCKSAMPLINGMODULE_H
#define MOCKSAMPLINGMODULE_H

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include <sndfile.h>
#include <thread>
#include <atomic>

class MockSamplingModule
{
public:
    using SampleCallback = std::function<void(const std::vector<float> &)>;

    MockSamplingModule(const std::string &wavFilePath);
    ~MockSamplingModule();
    void start();
    void stop();
    void registerCallback(SampleCallback callback);
    int getNumChannels() const;

private:
    void readWavFile();
    void processAudio();

    std::vector<SampleCallback> callbacks;
    std::vector<float> samples;
    std::string wavFilePath;
    std::atomic<bool> running;
    std::thread audioThread;
    int numChannels;
};

#endif // MOCKSAMPLINGMODULE_H