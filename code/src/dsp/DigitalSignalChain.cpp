#include "DigitalSignalChain.h"
#include "EffectFactory.h"
#include <iostream>
#include <cstring>
#include <unordered_set>

// Constructor: initialise both chains to empty slots
DigitalSignalChain::DigitalSignalChain()
{
    for (auto &chain : chains)
    {
        for (auto &slot : chain.effects)
        {
            slot.effect.reset();
            std::memset(slot.name, 0, sizeof(slot.name));
        }
        chain.count = 0;
    }
    activeChainIndex.store(0);
    registerAllEffects(); // Register all effects into active chain
}

// Register all effects from the factory
void DigitalSignalChain::registerAllEffects()
{
    size_t active = activeChainIndex.load();
    Chain &chain = chains[active];
    chain.count = 0;

    auto &factory = EffectFactory::instance();
    auto allNames = factory.getAllRegisteredEffectNames();

    std::unordered_set<std::string> registered;

    for (const auto &effectName : allNames)
    {
        if (chain.count >= MAX_EFFECTS)
            break;

        // Avoid duplicates
        if (!registered.insert(effectName).second)
            continue;

        auto effectPtr = factory.createEffect(effectName);
        if (effectPtr)
        {
            chain.effects[chain.count].effect = effectPtr;
            std::strncpy(chain.effects[chain.count].name, effectName.c_str(), sizeof(chain.effects[chain.count].name) - 1);
            chain.count++;
            std::cout << "[DigitalSignalChain] Registered effect: " << effectName << "\n";
        }
        else
        {
            std::cerr << "[DigitalSignalChain] Failed to create effect: " << effectName << "\n";
        }
    }
}

// Applies active effects to a sample
void DigitalSignalChain::applyEffects(Sample &sample)
{
    float originalValue = sample.getPcmValue();
    size_t activeIndex = activeChainIndex.load();
    const Chain &chain = chains[activeIndex];

    try
    {
        for (size_t i = 0; i < chain.count; ++i)
        {
            const auto &slot = chain.effects[i];
            if (!slot.effect || !slot.effect->isActive())
            {
                //std::cout << "[DigitalSignalChain] Skipping process...\n Value: " << originalValue << "\n";
                continue;
            }
            else
            {
                float result = slot.effect->process(sample.getPcmValue());
                sample.setPcmValue(result);
                sample.addEffect(slot.name);
            }
        }
    }
    catch (...)
    {
        sample.setPcmValue(originalValue); // fail-safe
    }
}

// Apply configuration to each effect
void DigitalSignalChain::configureEffects(Config &config)
{
    if (!config.hasUpdate())
        return;

    size_t activeIndex = activeChainIndex.load();
    Chain &chain = chains[activeIndex];

    std::cout << "[DigitalSignalChain] Configuring " << chain.count << " effect(s)\n";

    for (size_t i = 0; i < chain.count; ++i)
    {
        auto &slot = chain.effects[i];
        if (!slot.effect)
            continue;

        try
        {
            slot.effect->configure(config);
            std::cout << "[DigitalSignalChain] Configured: " << slot.name << "\n";
        }
        catch (const std::exception &e)
        {
            std::cerr << "[DigitalSignalChain] Error configuring " << slot.name << ": " << e.what() << "\n";
        }
    }

    config.clearUpdate(); // Clear update flag after successful configuration
}
