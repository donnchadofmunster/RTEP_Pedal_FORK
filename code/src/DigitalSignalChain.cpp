#include "DigitalSignalChain.h"

void DigitalSignalChain::registerEffect(std::shared_ptr<Effect> effect)
{
    effects.push_back(effect);
}

float DigitalSignalChain::applyEffects(float sample)
{
    for (const auto &effect : effects)
    {
        sample = effect->process(sample); // Ensure Effect has a valid `process()` method
    }
    return sample;
}
