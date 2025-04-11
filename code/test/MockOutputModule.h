#pragma once

#include <string>
#include <vector>
#include "Sample.h"
#include <sndfile.h>

class MockOutputModule {
public:
    MockOutputModule(const std::string& outputPath);
    void writeSample(const Sample& sample);
    void saveToFile();

private:
    std::vector<float> pcmSamples;
    std::string outputPath;
};
