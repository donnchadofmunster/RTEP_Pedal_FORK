#include "DigitalSignalChain.h"

void DigitalSignalChain::registerEffect(std::shared_ptr<Effect> effect)
{
    effects.registerEffect(effect);
}

void DigitalSignalChain::applyEffects(float &sample)
{
    for (auto &effect : effects)
    {
        effect->process(sample);
    }
}