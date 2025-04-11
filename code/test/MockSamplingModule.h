#pragma once

#include <string>
#include <vector>
#include <sndfile.h>

class MockSamplingModule {
public:
    MockSamplingModule(const std::string& filePath);
    bool getSample(float& outSample); // Returns true if a sample is available, false if done

private:
    void loadWavFile();

    std::vector<float> samples;
    std::size_t sampleIndex = 0;
    int numChannels = 0;
};
