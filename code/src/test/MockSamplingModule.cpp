#include "MockSamplingModule.h"
#include <iostream>

MockSamplingModule::MockSamplingModule(const std::string& filePath) {
    SF_INFO sfinfo = {};
    SNDFILE* file = sf_open(filePath.c_str(), SFM_READ, &sfinfo);
    
    if (!file) {
        std::cerr << "[MockSamplingModule] Failed to open WAV file: " << sf_strerror(nullptr) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    numChannels = sfinfo.channels;
    std::size_t totalSamples = sfinfo.frames * numChannels;

    samples.resize(totalSamples);
    sf_count_t readCount = sf_read_float(file, samples.data(), totalSamples);

    if (readCount != static_cast<sf_count_t>(totalSamples)) {
        std::cerr << "[MockSamplingModule] Incomplete WAV file read.\n";
        std::exit(EXIT_FAILURE);
    }

    sf_close(file);
}

bool MockSamplingModule::getSample(float& outSample) {
    if (sampleIndex >= samples.size()) {
        return false;
    }

    // Downmix to mono if necessary
    float mono = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch) {
        mono += samples[sampleIndex++];
    }
    mono /= numChannels;
    outSample = mono;

    return true;
}
