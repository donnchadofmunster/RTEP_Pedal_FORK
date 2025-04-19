#include <iostream>
#include <iomanip>
#include <csignal>
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include "MockSamplingModule.h"
#include "MockOutputModule.h"
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"
#include "Config.h"
#include "EncoderHandler.h"
#include "gpioevent.h"

// Optional linker anchor — ensures effects get registered
extern void ForceAllEffects(); // defined in ForceEffects.cpp

constexpr double SAMPLE_RATE = 44100.0;
constexpr double TIME_STEP = 1.0 / SAMPLE_RATE;

/**
 * @brief Processes a single sample, applies DSP, and writes to output
 */
void processSample(Sample &sample, DigitalSignalChain &dspChain, MockOutputModule &output)
{
    dspChain.applyEffects(sample);
    output.writeSample(sample);
}

/**
 * @brief Main test harness for processing a WAV through real-time effects
 */
int main(int argc, char *argv[])
{
    std::cout << "[test.cpp] Real-Time Harmoniser Pedal - Integration Test\n";

    if (argc < 3)
    {
        std::cerr << "[Usage] " << argv[0] << " <input.wav> <output.wav>\n";
        return EXIT_FAILURE;
    }

    // === Parse args ===
    const std::string inputWavFilePath = argv[1];
    const std::string outputWavFilePath = argv[2];

    // === Initialise system ===
    ForceAllEffects();                      // Register all effects
    Config &config = Config::getInstance(); // Singleton config
    config.registerSignalHandler();         // Enable SIGUSR1 update
    DigitalSignalChain dspChain;            // Create chain (auto-registers all effects)
    config.loadFromFile("./assets/config.cfg");
    dspChain.configureEffects(config);      // Apply initial configuration

    MockSamplingModule input(inputWavFilePath);
    MockOutputModule output(outputWavFilePath);

    float pcm;
    double timeIndex = 0.0;

    while (input.getSample(pcm))
    {
        if (config.hasUpdate())
        {
            std::cout << "\n[test.cpp] Config updated! Reconfiguring effects...\n";
            dspChain.configureEffects(config);
        }

        Sample sample(pcm, timeIndex);
        processSample(sample, dspChain, output);
        timeIndex += TIME_STEP;
    }

    std::cout << "\n[test.cpp] All samples processed. Writing output file...\n";
    output.saveToFile();
    std::cout << "[test.cpp] Done.\n";

    return EXIT_SUCCESS;
}
