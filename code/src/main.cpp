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
constexpr snd_pcm_uframes_t FRAMES = 44; // ~1ms
constexpr int BUFFER_SIZE = FRAMES * CHANNELS;

// void processBuffer(Sample* buffer, size_t bufferLength, DigitalSignalChain& dspChain, float setting, snd_pcm_t *playbackHandle)
// {
//     for (size_t i = 0; i < bufferLength; ++i) {
//         dspChain.applyEffects(buffer[i], setting);
//         std::cout << "\r" << std::flush;
//     }
//     snd_pcm_writei(playbackHandle, buffer, FRAMES);
// }

Sample processSample(Sample& sample, DigitalSignalChain& dspChain, float setting, snd_pcm_t *playbackHandle)
{
    dspChain.applyEffects(sample, setting);
    std::cout << "\r" << std::flush;
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
    printf("Anus\n");
    // snd_pcm_open(&captureHandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_set_params(captureHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                       CHANNELS, SAMPLE_RATE, 1, 1000);

    // snd_pcm_open(&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(playbackHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                       CHANNELS, SAMPLE_RATE, 1, 1000);

    std::cout << "Real-time capture → DSP → playback (~1ms chunks)\n";
    while (true) {
        snd_pcm_readi(captureHandle, buffer, FRAMES);
        for (snd_pcm_uframes_t i = 0; i < BUFFER_SIZE; ++i) {
            timeIndex += timeStep;
            Sample sample(buffer[i], timeIndex);
            sample = processSample(sample, dspChain, 2.0, playbackHandle);
        }
        
        // processBuffer(buffer, BUFFER_SIZE, dspChain, 2.0, playbackHandle);
        snd_pcm_writei(playbackHandle, buffer, FRAMES);
    }

    snd_pcm_close(captureHandle);
    snd_pcm_close(playbackHandle);
    return 0;
}





// #include <alsa/asoundlib.h>
// #include <sndfile.h>
// #include <iostream>

// constexpr unsigned int SAMPLE_RATE = 44100;
// constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16_LE;
// constexpr unsigned int CHANNELS = 1;
// constexpr snd_pcm_uframes_t FRAMES = 44;
// constexpr int BUFFER_SIZE = FRAMES * CHANNELS;

// int main() {
//     snd_pcm_t *captureHandle, *playbackHandle;
//     int16_t buffer[BUFFER_SIZE];

//     // Open input device
//     snd_pcm_open(&captureHandle, "default", SND_PCM_STREAM_CAPTURE, 0);
//     snd_pcm_set_params(captureHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
//                        CHANNELS, SAMPLE_RATE, 1, 1000);

//     snd_pcm_open(&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
//     snd_pcm_set_params(playbackHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
//                        CHANNELS, SAMPLE_RATE, 1, 1000);

//     // Open output WAV file
//     SF_INFO sfinfo = { SAMPLE_RATE, CHANNELS, SF_FORMAT_WAV | SF_FORMAT_PCM_16 };
//     SNDFILE *outfile = sf_open("out.wav", SFM_WRITE, &sfinfo);

//     std::cout << "Recording and playing... Press Ctrl+C to stop.\n";
//     while (true) {
//         snd_pcm_readi(captureHandle, buffer, FRAMES);
//         snd_pcm_writei(playbackHandle, buffer, FRAMES);
//         sf_writef_short(outfile, buffer, FRAMES);
//     }

//     sf_close(outfile);
//     snd_pcm_close(captureHandle);
//     snd_pcm_close(playbackHandle);
//     return 0;
// }
