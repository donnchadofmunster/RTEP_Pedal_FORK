#include <alsa/asoundlib.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <csignal>
#include <sys/signalfd.h>
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"
#include "Config.h"

extern void ForceAllEffects();

constexpr unsigned int SAMPLE_RATE = 44100;
constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16_LE;
constexpr unsigned int CHANNELS = 1;
constexpr snd_pcm_uframes_t FRAMES = 11;
constexpr int BUFFER_SIZE = FRAMES * CHANNELS;

Sample processSample(Sample &sample, DigitalSignalChain &dspChain)
{
    dspChain.applyEffects(sample);
    return sample;
}

/**
 * @brief Dedicated thread that blocks on SIGUSR1 using signalfd and updates effects.
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

        std::cerr << "[ConfigThread] SIGUSR1 received. Reconfiguring effects.\n";
        dspChain.configureEffects(config);
    }
}

int main()
{
    snd_pcm_t *captureHandle, *playbackHandle;
    int16_t buffer[BUFFER_SIZE];

    double timeIndex = 0.0;
    const double timeStep = 1.0 / SAMPLE_RATE;

    // Block SIGUSR1 in this thread (main + audio)
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &mask, nullptr);

    // Initialise and preload effects
    std::cout << "[Init] Registering and loading effects\n";
    ForceAllEffects();
    DigitalSignalChain dspChain;
    
    // Launch the config signal thread
    std::thread configThread(configUpdateThread, std::ref(dspChain));

    // Open ALSA devices
    snd_pcm_open(&captureHandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_open(&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(captureHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                       CHANNELS, SAMPLE_RATE, 1, 1000);
    snd_pcm_set_params(playbackHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                       CHANNELS, SAMPLE_RATE, 1, 1000);

    std::cout << "Starting real-time audio loop...\n";

    // Real-time loop
    while (true)
    {
        snd_pcm_readi(captureHandle, buffer, FRAMES);

        for (snd_pcm_uframes_t i = 0; i < BUFFER_SIZE; ++i)
        {
            timeIndex += timeStep;
            Sample sample(buffer[i], timeIndex);
            buffer[i] = processSample(sample, dspChain).getPcmValue();
        }

        snd_pcm_writei(playbackHandle, buffer, FRAMES);
    }

    configThread.join();
    snd_pcm_close(captureHandle);
    snd_pcm_close(playbackHandle);
    return 0;
}
