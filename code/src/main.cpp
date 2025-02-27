#include <cstdio>
#include "MockSamplingModule.h"
#include <iostream>

void processSample(const std::vector<float> &samples)
{
    std::cout << "Processing " << samples.size() << " samples." << std::endl;

    // Output each sample value
    for (size_t i = 0; i < samples.size(); ++i)
    {
        std::cout << "Sample " << i << ": " << samples[i] << std::endl;
    }
}

int main()
{
    printf("Real Time Harmoniser Pedal\n");

    std::string wavFilePath = "code/assets/input.wav"; // Provide the correct WAV file path
    MockSamplingModule mockSampler(wavFilePath);
    mockSampler.registerCallback(processSample);
    mockSampler.start();

    // Simulate running for a short period
    std::this_thread::sleep_for(std::chrono::seconds(2));

    mockSampler.stop();

    return 0;
}
