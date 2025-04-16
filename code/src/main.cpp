#include <alsa/asoundlib.h>
#include <iostream>
#include <iostream>
#include <iomanip>
#include <string>
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"
#include "OctaveDoubler.h"

extern void ForceAllEffects();  // defined in ForceEffects.cpp
constexpr unsigned int SAMPLE_RATE = 44100;
constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16_LE;
constexpr unsigned int CHANNELS = 1;
constexpr snd_pcm_uframes_t FRAMES = 11; // ~1ms
constexpr int BUFFER_SIZE = FRAMES * CHANNELS;

Sample processSample(Sample& sample, DigitalSignalChain& dspChain, float setting, snd_pcm_t *playbackHandle)
{
    dspChain.applyEffects(sample, setting);
    return sample;
}

int main() {
    snd_pcm_t *captureHandle, *playbackHandle;
    int16_t buffer[BUFFER_SIZE];
    DigitalSignalChain dspChain;

    double timeIndex = 0.0;
    constexpr double sampleRate = 44100.0;
    constexpr double timeStep = 1.0 / sampleRate;

    printf("Right before ForceAllEffects\n") ;
    ForceAllEffects();
    dspChain.loadEffectsFromFile("assets/effects_chain.txt");
    printf("Right after ForceAllEffects\n") ;

    snd_pcm_open(&captureHandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_open(&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(captureHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                       CHANNELS, SAMPLE_RATE, 1, 1000);

    snd_pcm_set_params(playbackHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                       CHANNELS, SAMPLE_RATE, 1, 1000);

    std::cout << "Real-time capture → DSP → playback (~1ms chunks)\n";
    while (true) {
        snd_pcm_readi(captureHandle, buffer, FRAMES);
        for (snd_pcm_uframes_t i = 0; i < BUFFER_SIZE; ++i) {
            timeIndex += timeStep;
            Sample sample(buffer[i], timeIndex);
            buffer[i] = processSample(sample, dspChain, 0.0, playbackHandle).getPcmValue();
        }
        snd_pcm_writei(playbackHandle, buffer, FRAMES);
    }

    snd_pcm_close(captureHandle);
    snd_pcm_close(playbackHandle);
    return 0;
}