#include <cstdio>
#include "MockSamplingModule.h"
#include "MockOutputModule.h"
#include "DigitalSignalChain.h"
#include "Sample.h"
#include <iostream>
#include <iomanip>
#include <thread>

void processSample(Sample &sample, DigitalSignalChain &dspChain, MockOutputModule &mockOutput)
{
    dspChain.applyEffects(sample); // Apply real-time effects

    std::cout << "Time: " << sample.getTimeIndex() << "s, PCM: "
              << std::fixed << std::setprecision(4) << sample.getPcmValue()
              << " Effects: ";

    for (const auto &effect : sample.getAppliedEffects())
    {
        std::cout << effect << " ";
    }
    std::cout << "\r" << std::flush;

    mockOutput.writeSample(sample); // Stream sample immediately
}

int main()
{
    printf("Real-Time Harmoniser Pedal: Testing Mode\n");

    std::string inputWavFilePath = "code/assets/input2.wav";
    std::string outputWavFilePath = "code/assets/output.wav";

    MockSamplingModule mockSampler(inputWavFilePath);
    MockOutputModule mockOutput(outputWavFilePath);
    DigitalSignalChain dspChain;

    // Capture number of channels
    // int numChannels = mockSampler.getNumChannels();

    mockSampler.registerCallback([&dspChain, &mockOutput](const Sample &sample)
                                 { processSample(const_cast<Sample &>(sample), dspChain, mockOutput); });

    mockSampler.start();
    mockOutput.start();

    // Keep running while the sampler is active
    while (mockSampler.isRunning())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    mockSampler.stop();
    mockOutput.stop();

    return 0;
}
