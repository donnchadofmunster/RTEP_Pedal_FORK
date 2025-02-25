#include "config.h"
#include "DisplayHandler.h"
#include "SamplingModule.h"
#include "DigitalSignalChain.h"
#include "OutputModule.h"

int main()
{
    Config config;
    DisplayHandler displayHandler;
    SamplingModule sampler;
    DigitalSignalChain dspChain;
    OutputModule output;

    float sample;
    sampler.captureAudio(sample);
    dspChain.registerEffect(config.getEffect());
    dspChain.applyEffects(sample);
    output.outputSample(sample);
    displayHandler.updateDisplay(config);
    return 0;
}