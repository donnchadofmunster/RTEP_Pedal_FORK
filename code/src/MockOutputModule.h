#ifndef MOCKOUTPUTMODULE_H
#define MOCKOUTPUTMODULE_H

#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>

class MockOutputModule
{
public:
    using OutputCallback = std::function<void(const std::vector<float> &, int)>;

    MockOutputModule();
    ~MockOutputModule();
    void start();
    void stop();
    void registerCallback(OutputCallback callback);
    void writeSamples(const std::vector<float> &samples, int numChannels);

private:
    void processOutput();

    std::vector<OutputCallback> callbacks;
    std::vector<float> outputBuffer;
    int numChannels;
    std::atomic<bool> running;
    std::thread outputThread;
};

#endif // MOCKOUTPUTMODULE_H