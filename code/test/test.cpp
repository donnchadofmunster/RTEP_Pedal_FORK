#include <cstdio>
#include "MockSamplingModule.h"
#include "MockOutputModule.h"
#include "DigitalSignalChain.h"
#include <iostream>
#include <iomanip>
#include <thread>

void processSample(std::vector<float> &samples, int numChannels, DigitalSignalChain &dspChain, MockOutputModule &mockOutput)
{
    std::cout << "Processing " << samples.size() / numChannels << " frames with " << numChannels << " channels.\n"
              << std::flush;

    // Apply effects to each sample
    for (size_t i = 0; i < samples.size(); ++i)
    {
        samples[i] = dspChain.applyEffects(samples[i]);
    }

    // Output each sample value per channel
    for (size_t i = 0; i < samples.size(); i += numChannels)
    {
        std::cout << "Frame " << i / numChannels << ": ";
        for (int ch = 0; ch < numChannels; ++ch)
        {
            std::cout << "Ch" << ch << "=" << std::fixed << std::setprecision(4) << samples[i + ch] << " ";
        }
        std::cout << "\r" << std::flush;
    }

    // Pass the processed samples to the mock output module
    mockOutput.writeSamples(samples, numChannels);
}

int main()
{
    printf("Real Time Harmoniser Pedal: Testing Mode\n");

    std::string inputWavFilePath = "code/assets/input.wav";
    std::string outputWavFilePath = "code/assets/output.wav";

    MockSamplingModule mockSampler(inputWavFilePath);
    MockOutputModule mockOutput(outputWavFilePath);
    DigitalSignalChain dspChain;

    // Capture number of channels
    int numChannels = mockSampler.getNumChannels();

    mockSampler.registerCallback([numChannels, &dspChain, &mockOutput](const std::vector<float> &samples){
        std::vector<float> mutableSamples = samples;
        processSample(mutableSamples, numChannels, dspChain, mockOutput);
    });

    mockSampler.start();
    mockOutput.start();

    // Simulate running for a short period
    std::this_thread::sleep_for(std::chrono::seconds(2));

    mockSampler.stop();
    mockOutput.stop();

    return 0;
}
