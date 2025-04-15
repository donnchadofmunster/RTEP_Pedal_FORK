#include <alsa/asoundlib.h>
#include <sndfile.h>
#include <iostream>

constexpr unsigned int SAMPLE_RATE = 44100;
constexpr snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16_LE;
constexpr unsigned int CHANNELS = 1;
constexpr snd_pcm_uframes_t FRAMES = 44;
constexpr int BUFFER_SIZE = FRAMES * CHANNELS;

int main() {
    snd_pcm_t *captureHandle, *playbackHandle;
    int16_t buffer[BUFFER_SIZE];

    // Open input device
    snd_pcm_open(&captureHandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_set_params(captureHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                       CHANNELS, SAMPLE_RATE, 1, 1000);

    snd_pcm_open(&playbackHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(playbackHandle, FORMAT, SND_PCM_ACCESS_RW_INTERLEAVED,
                       CHANNELS, SAMPLE_RATE, 1, 1000);

    // Open output WAV file
    SF_INFO sfinfo = { SAMPLE_RATE, CHANNELS, SF_FORMAT_WAV | SF_FORMAT_PCM_16 };
    SNDFILE *outfile = sf_open("out.wav", SFM_WRITE, &sfinfo);

    std::cout << "Recording and playing... Press Ctrl+C to stop.\n";
    while (true) {
        snd_pcm_readi(captureHandle, buffer, FRAMES);
        snd_pcm_writei(playbackHandle, buffer, FRAMES);
        sf_writef_short(outfile, buffer, FRAMES);
    }

    sf_close(outfile);
    snd_pcm_close(captureHandle);
    snd_pcm_close(playbackHandle);
    return 0;
}
