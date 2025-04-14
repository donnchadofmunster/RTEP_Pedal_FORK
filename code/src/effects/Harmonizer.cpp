#include "Harmonizer.h"
#include "EffectRegistration.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <algorithm>

// Constructor that resolves file paths relative to "assets/"
Harmonizer::Harmonizer(const std::string &inputWav, const std::string &outputWav, const std::vector<int> &semitones)
    : inputWav("assets/" + inputWav),
      outputWav("assets/" + outputWav),
      semitones(semitones) {}

// Update internal paths and pitch settings
void Harmonizer::updateInputs(const std::string &in, const std::string &out, const std::vector<int> &newSemitones)
{
    inputWav = in;
    outputWav = out;
    semitones = newSemitones;
}

// Initialise the sample-by-sample stretching module
void Harmonizer::initRealtimeStretch()
{
    if (stretchInitialized)
        return;
    stretch.presetDefault(1, sampleRate);
    inputBuffer.resize(blockSize);
    outputBuffer.assign(blockSize, 0.0f);
    inputWriteIndex = 0;
    outputReadIndex = outputBuffer.size();
    stretchInitialized = true;
}

// Sample-by-sample realtime processing entry point
float Harmonizer::process(float sample, float setting)
{
    initRealtimeStretch();

    if (samplesProcessed++ == 0)
    {
        realtimeStart = std::chrono::high_resolution_clock::now();
    }

    inputBuffer[inputWriteIndex++] = sample;

    // Return from filled output buffer
    if (outputReadIndex < outputBuffer.size())
    {
        return outputBuffer[outputReadIndex++];
    }

    // Refill output buffer when input buffer is full
    if (inputWriteIndex >= blockSize)
    {
        if (lastSemitoneSetting != setting)
        {
            stretch.setTransposeSemitones(setting, tonality / sampleRate);
            lastSemitoneSetting = setting;
        }

        float *inputs[1] = {inputBuffer.data()};
        float *outputs[1] = {outputBuffer.data()};
        stretch.process(inputs, blockSize, outputs, blockSize);

        inputWriteIndex = 0;
        outputReadIndex = 0;
        return outputBuffer[outputReadIndex++];
    }

    // Return silence if buffer is still filling
    return 0.0f;
}

// Offline stretch setup for file-based processing
void Harmonizer::setupStretch(int currentSemitone)
{
    if (!inWav.read(inputWav).warn())
    {
        std::cerr << "Error reading input WAV file!" << std::endl;
        return;
    }

    const size_t inputLength = inWav.samples.size() / inWav.channels;
    outWav.channels = inWav.channels;
    outWav.sampleRate = inWav.sampleRate;

    const int outputLength = std::round(inputLength * time);
    stretch.presetDefault(inWav.channels, inWav.sampleRate);
    stretch.setTransposeSemitones(currentSemitone, tonality / inWav.sampleRate);

    inWav.samples.resize((inputLength + stretch.inputLatency()) * inWav.channels);
    const int tailSamples = exactLength ? stretch.outputLatency() : (stretch.outputLatency() + stretch.inputLatency());
    outWav.samples.resize((outputLength + tailSamples) * outWav.channels);
}

// Process audio blocks for offline rendering
void Harmonizer::processAudio()
{
    const size_t inputLength = inWav.samples.size() / inWav.channels;
    const size_t outputLength = std::round(inputLength * time);

    stretch.seek(inWav, stretch.inputLatency(), 1 / time);
    inWav.offset += stretch.inputLatency();

    stretch.process(inWav, inputLength, outWav, outputLength);
    outWav.offset += outputLength;
    stretch.flush(outWav, stretch.outputLatency());
    outWav.offset -= outputLength;
}

// Entry point for offline harmonisation (used in chord creation)
bool Harmonizer::process(int iteration)
{
    setupStretch(semitones[iteration]);
    std::cout << "Processing " << inputWav << " with " << semitones[iteration] << " semitones\n";

    processAudio();

    outputWav = "assets/output" + std::to_string(iteration) + ".wav";
    outWav.write(outputWav);
    return true;
}

// Stats logger after file-based or streaming harmonisation
void Harmonizer::reportProcessingStats(double seconds, double rate, double cpu)
{
    std::cout << "Process:\n\t" << seconds << "s, " << rate << "x realtime, " << cpu << "% CPU\n";
    if (processMemory.implemented)
    {
        std::cout << "\tallocated " << (processMemory.allocBytes / 1000)
                  << "kB, freed " << (processMemory.freeBytes / 1000) << "kB\n";
    }
}

// Merges two WAVs into a single file
std::string Harmonizer::mergeWavs(const char *file1, const char *file2, const char *outfile)
{
    static double data1[1024], data2[1024], mixed[1024];
    SF_INFO sfinfo1 = {}, sfinfo2 = {};
    SNDFILE *in1 = sf_open(file1, SFM_READ, &sfinfo1);
    SNDFILE *in2 = sf_open(file2, SFM_READ, &sfinfo2);
    SNDFILE *out = sf_open(outfile, SFM_WRITE, &sfinfo1);

    int read1, read2;
    while ((read1 = sf_read_double(in1, data1, 1024)) &&
           (read2 = sf_read_double(in2, data2, 1024)))
    {
        for (int i = 0; i < read1; ++i)
        {
            mixed[i] = std::clamp((data1[i] + data2[i]) * 0.5, -1.0, 1.0);
        }
        sf_write_double(out, mixed, read1);
    }

    std::filesystem::remove(file1);
    std::filesystem::remove(file2);

    sf_close(in1);
    sf_close(in2);
    sf_close(out);
    return std::string(outfile);
}

// Mixes harmonised voices to build a chord
bool Harmonizer::createChord()
{
    stopwatch.start();

    std::vector<std::string> outputs;
    for (size_t i = 0; i < semitones.size(); ++i)
    {
        process(i);
        outputs.emplace_back("assets/output" + std::to_string(i) + ".wav");
    }

    std::string final = outputs[0];
    for (size_t i = 1; i < outputs.size(); ++i)
    {
        std::string merged = "assets/merged" + std::to_string(i) + ".wav";
        final = mergeWavs(final.c_str(), outputs[i].c_str(), merged.c_str());
    }

    std::filesystem::rename(final, "assets/output.wav");

    double seconds = stopwatch.seconds(stopwatch.lap());
    double rate = (inWav.length() / inWav.sampleRate) / seconds;
    double cpu = 100.0 / rate;
    reportProcessingStats(seconds, rate, cpu);
    return true;
}

// Dummy processing hook (placeholder)
void Harmonizer::data_processing(double *data, int count, int channels)
{
    for (int ch = 0; ch < channels; ++ch)
        for (int i = ch; i < count; i += channels)
            data[i] *= 1.0;
}

// Logs realtime performance stats on destruction
Harmonizer::~Harmonizer()
{
    using namespace std::chrono;

    if (samplesProcessed > 0)
    {
        auto end = high_resolution_clock::now();
        double elapsed = duration<double>(end - realtimeStart).count();
        double perSampleUs = (elapsed / samplesProcessed) * 1e6;
        double realtimeRatio = (samplesProcessed / static_cast<double>(sampleRate)) / elapsed;

        std::cout << "[Harmonizer] Real-time stretch stats:\n";
        std::cout << "\tSamples processed: " << samplesProcessed << "\n";
        std::cout << "\tElapsed time: " << elapsed << " s\n";
        std::cout << "\tPer sample: " << perSampleUs << " µs\n";
        std::cout << "\tRealtime factor: " << realtimeRatio << "x\n";
    }
}

REGISTER_EFFECT_AUTO(Harmonizer);
