#include "Harmonizer.h"
#include "EffectRegistration.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <algorithm>

Harmonizer::Harmonizer(const std::string &inputWav, const std::string &outputWav, const std::vector<int> &semitones)
    : inputWav("assets/" + inputWav),
      outputWav("assets/" + outputWav),
      semitones(semitones)
{
    IsActive = false;
}

void Harmonizer::updateInputs(const std::string &in, const std::string &out, const std::vector<int> &newSemitones)
{
    inputWav = in;
    outputWav = out;
    semitones = newSemitones;
}

void Harmonizer::initRealtimeStretch()
{
    if (stretchInitialized) return;

    stretches.resize(semitones.size());
    outputBuffers.resize(semitones.size());
    for (size_t i = 0; i < semitones.size(); ++i)
    {
        stretches[i].presetDefault(1, sampleRate);
        outputBuffers[i].assign(blockSize, 0.0f);
    }

    inputBuffer.resize(blockSize);
    inputWriteIndex = 0;
    outputReadIndex = blockSize;
    stretchInitialized = true;
}

float Harmonizer::process(float sample)
{
    if (!IsActive)
    {
        return sample;
    }
    initRealtimeStretch();

    if (samplesProcessed++ == 0)
    {
        realtimeStart = std::chrono::high_resolution_clock::now();
    }

    inputBuffer[inputWriteIndex++] = sample;

    if (outputReadIndex < blockSize)
    {
        float mixed = 0.0f;
        for (const auto &buffer : outputBuffers)
        {
            mixed += buffer[outputReadIndex];
        }
        ++outputReadIndex;
        return mixed / outputBuffers.size();
    }

    if (inputWriteIndex >= blockSize)
    {

        float *inputs[1] = {inputBuffer.data()};
        for (size_t i = 0; i < semitones.size(); ++i)
        {
            stretches[i].setTransposeSemitones(semitones[i], tonality / sampleRate);
            float *outputs[1] = {outputBuffers[i].data()};
            stretches[i].process(inputs, blockSize, outputs, blockSize);
        }

        inputWriteIndex = 0;
        outputReadIndex = 0;

        float mixed = 0.0f;
        for (const auto &buffer : outputBuffers)
        {
            mixed += buffer[outputReadIndex];
        }
        ++outputReadIndex;
        return mixed / outputBuffers.size();
    }

    return 0.0f;
}

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

bool Harmonizer::process(int iteration)
{
    setupStretch(semitones[iteration]);
    std::cout << "Processing " << inputWav << " with " << semitones[iteration] << " semitones\n";

    processAudio();

    outputWav = "assets/output" + std::to_string(iteration) + ".wav";
    outWav.write(outputWav);
    return true;
}

void Harmonizer::data_processing(double *data, int count, int channels)
{
    for (int ch = 0; ch < channels; ++ch)
        for (int i = ch; i < count; i += channels)
            data[i] *= 1.0;
}

void Harmonizer::parseConfig(const Config &config)
{
    IsActive = config.contains("harmonizer");

    std::string intervalsStr = config.get<std::string>("harmonizer", "0");
    std::stringstream ss(intervalsStr);
    std::string token;
    semitones.clear();

    std::cout << "[Harmonizer] Raw interval string: \"" << intervalsStr << "\"\n";

    while (ss >> token) // space-separated
    {
        try
        {
            int val = std::stoi(token);
            semitones.push_back(val);
            std::cout << "[Harmonizer] Parsed semitone: " << val << "\n";
        }
        catch (...)
        {
            std::cerr << "[Harmonizer] Warning: invalid interval \"" << token << "\"\n";
        }
    }

    std::cout << "[Harmonizer] Total intervals loaded: " << semitones.size() << "\n";
}

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
