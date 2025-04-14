#include "DigitalSignalChain.h"
#include "EffectFactory.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <mutex>
#include <cstring>

// Constructor initializes both signal chains with empty effect slots
DigitalSignalChain::DigitalSignalChain()
{
    for (auto &chain : chains)
    {
        for (auto &slot : chain.effects)
        {
            slot.effect.reset(); // use smart pointer
            std::memset(slot.name, 0, sizeof(slot.name));
        }
        chain.count = 0;
    }
    activeChainIndex.store(0); // default to first chain
}

// Loads effect names from file and builds an inactive chain, then swaps it in
bool DigitalSignalChain::loadEffectsFromFile(const std::string &filepath)
{
    std::ifstream infile(filepath);
    if (!infile.is_open())
    {
        std::cerr << "[DigitalSignalChain] Failed to open effect config file: " << filepath << std::endl;
        return false;
    }

    size_t inactive = (activeChainIndex.load() + 1) % 2;
    Chain &chain = chains[inactive];
    chain.count = 0;

    std::string effectName;
    while (std::getline(infile, effectName) && chain.count < MAX_EFFECTS)
    {
        // Trim leading/trailing whitespace
        effectName.erase(0, effectName.find_first_not_of(" \t\n\r"));
        effectName.erase(effectName.find_last_not_of(" \t\n\r") + 1);
        if (effectName.empty())
            continue;

        std::cout << "[DigitalSignalChain] Loading effect: " << effectName << std::endl;
        auto effectPtr = EffectFactory::instance().createEffect(effectName);
        if (effectPtr)
        {
            chain.effects[chain.count].effect = effectPtr; // smart pointer
            std::strncpy(chain.effects[chain.count].name, effectName.c_str(), sizeof(chain.effects[chain.count].name) - 1);
            chain.count++;
            std::cout << "[DigitalSignalChain] Registered effect: " << effectName << std::endl;
        }
        else
        {
            std::cerr << "[DigitalSignalChain] Unknown effect: " << effectName << std::endl;
        }
    }

    activeChainIndex.store(inactive);
    return true;
}

// Applies the effect chain to an audio sample
void DigitalSignalChain::applyEffects(Sample &sample, float setting)
{
    float originalValue = sample.getPcmValue();
    size_t activeIndex = activeChainIndex.load();
    const Chain &chain = chains[activeIndex];

    try
    {
        for (size_t i = 0; i < chain.count; ++i)
        {
            const auto &slot = chain.effects[i];
            if (!slot.effect)
                continue;

            float result = slot.effect->process(sample.getPcmValue(), setting);
            sample.setPcmValue(result);
            sample.addEffect(slot.name);
        }
    }
    catch (...)
    {
        sample.setPcmValue(originalValue); // Recover in case of exception
    }
}
