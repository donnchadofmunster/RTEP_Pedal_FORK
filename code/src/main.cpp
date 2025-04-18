#include <iostream>
#include <iomanip>
#include <thread>
#include <csignal>
#include <sys/signalfd.h>
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"
#include "Config.h"
#include "AudioIO.h" // New audio abstraction layer

extern void ForceAllEffects();

constexpr unsigned int SAMPLE_RATE = 44100;
constexpr unsigned int FRAMES = 11;
constexpr int BUFFER_SIZE = FRAMES; // Mono (1 channel), already interleaved

/**
 * @brief Processes a single audio sample through the DSP chain.
 *
 * @param sample The audio sample object.
 * @param dspChain The configured DSP chain.
 * @return The processed sample.
 */
Sample processSample(Sample &sample, DigitalSignalChain &dspChain)
{
    dspChain.applyEffects(sample);
    return sample;
}

/**
 * @brief Thread that blocks on SIGUSR1 using signalfd and triggers configuration reload.
 *
 * @param dspChain The digital signal chain to reconfigure.
 */
void configUpdateThread(DigitalSignalChain &dspChain)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    int sfd = signalfd(-1, &mask, 0);
    if (sfd == -1)
    {
        perror("signalfd");
        return;
    }

    Config &config = Config::getInstance();

    while (true)
    {
        struct signalfd_siginfo fdsi;
        ssize_t s = read(sfd, &fdsi, sizeof(fdsi));
        if (s != sizeof(fdsi))
            continue;

        std::cerr << "[ConfigThread] SIGUSR1 received. Reconfiguring effects...\n";
        dspChain.configureEffects(config);
    }
}

int main()
{
    // Block SIGUSR1 in main and audio thread
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &mask, nullptr);

    std::cout << "[Init] Registering and loading effects...\n";
    ForceAllEffects(); // Statically register all effects
    DigitalSignalChain dspChain;

    // Launch configuration watcher thread
    std::thread configThread(configUpdateThread, std::ref(dspChain));

    // Load initial configuration file (optional)
    Config &config = Config::getInstance();
    config.loadFromFile("config.cfg");
    dspChain.configureEffects(config);

    // Audio I/O module
    AudioIO audio;
    if (!audio.init())
    {
        std::cerr << "[AudioIO] Failed to initialise audio device.\n";
        return 1;
    }

    std::cout << "[Init] Starting real-time audio loop...\n";

    int16_t buffer[BUFFER_SIZE];
    double timeIndex = 0.0;
    const double timeStep = 1.0 / SAMPLE_RATE;

    while (true)
    {
        if (!audio.readBuffer(buffer))
        {
            std::cerr << "[AudioIO] Failed to read audio.\n";
            continue;
        }

        for (snd_pcm_uframes_t i = 0; i < BUFFER_SIZE; ++i)
        {
            timeIndex += timeStep;
            Sample sample(buffer[i], timeIndex);
            buffer[i] = processSample(sample, dspChain).getPcmValue();
        }

        if (!audio.writeBuffer(buffer))
        {
            std::cerr << "[AudioIO] Failed to write audio.\n";
            continue;
        }
    }

    audio.cleanup();
    configThread.join();
    return 0;
}
