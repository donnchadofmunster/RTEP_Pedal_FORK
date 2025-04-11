#include <iostream>
#include <iomanip>
#include <string>
#include "MockSamplingModule.h"
#include "MockOutputModule.h"
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"
#include "OctaveDoubler.h"

// Optional linker anchor for safety — may be removed if REGISTER_EFFECT_AUTO is working reliably.
extern void ForceAllEffects();  // defined in ForceEffects.cpp

/**
 * @brief Processes a single audio sample by applying all registered DSP effects,
 *        logging it to console, and writing it to the output module.
 *
 * @param sample The audio sample to process
 * @param dspChain The digital signal chain containing the effects
 * @param mockOutput The output module for writing processed samples
 */
void processSample(Sample& sample, DigitalSignalChain& dspChain, MockOutputModule& mockOutput)
{
    dspChain.applyEffects(sample);

    std::cout << "[test.cpp] Time: " << sample.getTimeIndex() << "s, PCM: "
              << std::fixed << std::setprecision(4) << sample.getPcmValue();

    for (const auto& effect : sample.getAppliedEffects()) {
        std::cout << " [" << effect << "]";
    }

    std::cout << "\r" << std::flush;

    mockOutput.writeSample(sample);
}

/**
 * @brief Entry point for the real-time harmoniser pedal test.
 * Loads an input WAV, registers a DSP effect, processes each sample, and writes to output.
 */
int main()
{
    std::cout << "Real-Time Harmoniser Pedal: Testing Mode\n";

    // File paths
    const std::string inputWavFilePath = "assets/input_440.wav";
    const std::string outputWavFilePath = "assets/output.wav";

    // Module construction
    MockSamplingModule mockSampler(inputWavFilePath);
    MockOutputModule mockOutput(outputWavFilePath);
    DigitalSignalChain dspChain;

    // Optional: call to anchor OctaveDoubler object file during linking (only needed if effect fails to register)
    ForceAllEffects();

    // Retrieve the "OctaveDoubler" effect registered via REGISTER_EFFECT_AUTO
    try {
        auto effect = EffectFactory::instance().createEffect("OctaveDoubler");
        if (!effect) {
            throw std::runtime_error("[test.cpp] OctaveDoubler is not registered.");
        }
        std::cout << "[test.cpp] OctaveDoubler successfully registered.\n";
        dspChain.registerEffect(effect);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Process audio sample-by-sample
    float pcm;
    double timeIndex = 0.0;
    constexpr double sampleRate = 44100.0;
    constexpr double timeStep = 1.0 / sampleRate;

    while (mockSampler.getSample(pcm)) {
        Sample sample(pcm, timeIndex);
        processSample(sample, dspChain, mockOutput);
        timeIndex += timeStep;
    }

    std::cout << "\n [test.cpp]  All samples processed. Writing output file...\n";
    mockOutput.saveToFile();

    std::cout << "[test.cpp]  Done.\n";
    return 0;
}
