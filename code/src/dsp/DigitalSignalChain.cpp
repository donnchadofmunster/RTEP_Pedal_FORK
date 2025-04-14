#include "DigitalSignalChain.h"
#include "EffectFactory.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <mutex>
#include <cstring>


DigitalSignalChain::DigitalSignalChain()
{
    // Initialize both chains as empty
    for (auto &chain : chains)
    {
        for (auto &slot : chain.effects)
        {
            slot.effect = nullptr;
            std::memset(slot.name, 0, sizeof(slot.name));
        }
        chain.count = 0;
    }
    activeChainIndex.store(0);
}

// Loads effects from file and prepares an inactive chain buffer for hot-swapping
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
        effectName.erase(0, effectName.find_first_not_of(" \t\n\r"));
        effectName.erase(effectName.find_last_not_of(" \t\n\r") + 1);

        if (effectName.empty())
            continue;

        std::cout << "[DigitalSignalChain] Loading effect: " << effectName << std::endl;
        auto effect = EffectFactory::instance().createEffect(effectName);
        if (effect)
        {
            chain.effects[chain.count].effect = effect; // Raw pointer is safe for real-time, effect is owned elsewhere
            std::strncpy(chain.effects[chain.count].name, effectName.c_str(), sizeof(chain.effects[chain.count].name) - 1);
            chain.count++;
            std::cout << "[DigitalSignalChain] Registered effect: " << effectName << std::endl;
        }
        else
        {
            std::cerr << "[DigitalSignalChain] Unknown effect: " << effectName << std::endl;
        }
    }

    // Atomically switch to the new chain
    activeChainIndex.store(inactive);
    return true;
}

// Apply all effects in the currently active chain
void DigitalSignalChain::applyEffects(Sample &sample, float setting)
{
    float pcmValue = sample.getPcmValue();

    size_t index = activeChainIndex.load();
    const Chain &chain = chains[index];

    try
    {
        for (size_t i = 0; i < chain.count; ++i)
        {
            auto &effect = chain.effects[i].effect;
            if (!effect)
                continue;

            float processed = effect->process(sample.getPcmValue(), setting);
            sample.setPcmValue(processed);
            sample.addEffect(chain.effects[i].name);
        }
    }
    catch (...)
    {
        sample.setPcmValue(pcmValue); // Fallback to original if error occurs
    }
}
