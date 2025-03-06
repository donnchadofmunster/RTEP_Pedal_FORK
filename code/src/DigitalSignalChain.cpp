#include "DigitalSignalChain.h"

void DigitalSignalChain::registerEffect(std::shared_ptr<Effect> effect)
{
    effects.push_back(effect);
}

void DigitalSignalChain::applyEffects(Sample &sample)
{
    float pcmValue = sample.getPcmValue(); // Store PCM value in a variable
    for (const auto &effect : effects)
    {
        pcmValue = effect->process(pcmValue);     // Process PCM value
        sample.addEffect(typeid(*effect).name()); // Track applied effect
    }
    sample.setPcmValue(pcmValue); // Update sample with processed value
}
