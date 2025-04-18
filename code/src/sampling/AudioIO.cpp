// AudioIO.cpp
#include "AudioIO.h"
#include <alsa/asoundlib.h>
#include <iostream>
#include "Sample.h"

constexpr unsigned int SAMPLE_RATE = 44100;
constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16_LE;
constexpr unsigned int CHANNELS = 1;
constexpr snd_pcm_uframes_t FRAMES = 11;
constexpr int BUFFER_SIZE = FRAMES * CHANNELS;

bool AudioIO::init()
{
    if (snd_pcm_open(&captureHandle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0)
        return false;
    if (snd_pcm_open(&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0)
        return false;

    if (snd_pcm_set_params(captureHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                           CHANNELS, SAMPLE_RATE, 1, 1000) < 0)
        return false;
    if (snd_pcm_set_params(playbackHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                           CHANNELS, SAMPLE_RATE, 1, 1000) < 0)
        return false;
    return true;
}

bool AudioIO::readBuffer(int16_t *buffer)
{
    return snd_pcm_readi(captureHandle, buffer, FRAMES) >= 0;
}

bool AudioIO::writeBuffer(int16_t *buffer)
{
    return snd_pcm_writei(playbackHandle, buffer, FRAMES) >= 0;
}

void AudioIO::cleanup()
{
    if (captureHandle)
        snd_pcm_close(captureHandle);
    if (playbackHandle)
        snd_pcm_close(playbackHandle);
}
