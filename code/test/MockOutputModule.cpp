#include "MockOutputModule.h"
#include <iostream>

MockOutputModule::MockOutputModule(const std::string& outputPath)
    : outputPath(outputPath) {}

void MockOutputModule::writeSample(const Sample& sample) {
    pcmSamples.push_back(sample.getPcmValue());
}

void MockOutputModule::saveToFile() {
    if (pcmSamples.empty()) {
        std::cerr << "[MockOutputModule] No samples to write.\n";
        return;
    }

    SF_INFO sfinfo = {};
    sfinfo.samplerate = 44100; // Default rate
    sfinfo.channels = 1;       // Mono output
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    SNDFILE* file = sf_open(outputPath.c_str(), SFM_WRITE, &sfinfo);
    if (!file) {
        std::cerr << "[MockOutputModule] Failed to open file for writing: " << sf_strerror(nullptr) << std::endl;
        return;
    }

    sf_write_float(file, pcmSamples.data(), pcmSamples.size());
    sf_close(file);

    std::cout << "[MockOutputModule] WAV written to: " << outputPath << std::endl;
}
