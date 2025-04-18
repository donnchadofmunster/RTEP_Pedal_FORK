// AudioIO.h
#ifndef AUDIO_IO_H
#define AUDIO_IO_H

#include <cstdint>
#include <alsa/asoundlib.h>

/**
 * @class AudioIO
 * @brief Handles ALSA-based audio input and output for real-time DSP applications.
 *
 * This class abstracts the setup, capture, and playback of audio samples using
 * the ALSA API. It is designed for real-time applications with low latency requirements.
 */
class AudioIO
{
public:
    /**
     * @brief Initializes the ALSA capture and playback devices with predefined parameters.
     * @return true if initialization succeeds; false otherwise.
     */
    bool init();

    /**
     * @brief Reads a block of audio samples from the ALSA capture device.
     * @param buffer A pointer to an int16_t buffer to store the input samples.
     * @return true if samples were successfully read; false otherwise.
     */
    bool readBuffer(int16_t *buffer);

    /**
     * @brief Writes a block of audio samples to the ALSA playback device.
     * @param buffer A pointer to an int16_t buffer containing the output samples.
     * @return true if samples were successfully written; false otherwise.
     */
    bool writeBuffer(int16_t *buffer);

    /**
     * @brief Closes the ALSA capture and playback devices safely.
     */
    void cleanup();

private:
    snd_pcm_t *captureHandle = nullptr;  ///< ALSA handle for the capture device.
    snd_pcm_t *playbackHandle = nullptr; ///< ALSA handle for the playback device.
};

#endif // AUDIO_IO_H
