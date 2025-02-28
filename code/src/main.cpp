#include <cstdio>
#include "MockSamplingModule.h"
#include <iostream>
#include <iomanip>

void processSample(const std::vector<float> &samples, int numChannels)
{
    std::cout << "Processing " << samples.size() / numChannels << " frames with " << numChannels << " channels.\n" << std::flush;

    // Output each sample value per channel, overwriting the line
    for (size_t i = 0; i < samples.size(); i += numChannels)
    {
        std::cout << "Frame " << i / numChannels << ": ";
        for (int ch = 0; ch < numChannels; ++ch)
        {
            std::cout << "Ch" << ch << "=" << std::fixed << std::setprecision(4) << samples[i + ch] << " ";
        }
        std::cout << "\r" << std::flush;
    }
}

int main()
{
    printf("Real Time Harmoniser Pedal\n");

    std::string wavFilePath = "code/assets/input.wav"; // Provide the correct WAV file path
    MockSamplingModule mockSampler(wavFilePath);

    // Capture number of channels
    int numChannels = mockSampler.getNumChannels();

    mockSampler.registerCallback([numChannels](const std::vector<float> &samples)
                                 { processSample(samples, numChannels); });

    mockSampler.start();

    // Simulate running for a short period
    std::this_thread::sleep_for(std::chrono::seconds(2));

    mockSampler.stop();

    return 0;
}
