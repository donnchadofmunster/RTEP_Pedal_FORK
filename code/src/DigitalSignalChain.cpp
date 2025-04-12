#include "DigitalSignalChain.h"
#include "EffectFactory.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <typeinfo>

bool DigitalSignalChain::loadEffectsFromFile(const std::string& filepath)
{
    std::ifstream infile(filepath);
    if (!infile.is_open()) {
        std::cerr << "[DigitalSignalChain] Failed to open effect config file: " << filepath << std::endl;
        return false;
    }

    std::string effectName;
    while (std::getline(infile, effectName)) {
        // Trim whitespace (optional)
        effectName.erase(0, effectName.find_first_not_of(" \t\n\r"));
        effectName.erase(effectName.find_last_not_of(" \t\n\r") + 1);

        if (effectName.empty()) continue;

        std::cout << "[DigitalSignalChain] Loading effect: " << effectName << std::endl;

        auto effect = EffectFactory::instance().createEffect(effectName);
        if (effect) {
            registerEffect(effect);
            std::cout << "[DigitalSignalChain] Registered effect: " << effectName << std::endl;
        } else {
            std::cerr << "[DigitalSignalChain] Unknown effect: " << effectName << std::endl;
        }
    }

    return true;
}

void DigitalSignalChain::registerEffect(std::shared_ptr<Effect> effect)
{
    if (!effect) {
        std::cerr << "Warning: Tried to register null effect\n";
        return;
    }
    effects.push_back(effect);
}

void DigitalSignalChain::applyEffects(Sample &sample, float setting = 3.0)
{
    float pcmValue = sample.getPcmValue();
    try{
            for (auto &effect : effects) {
        if (!effect) {
            std::cerr << "Null effect encountered in signal chain!\n";
            continue;
        }

        float processed = effect->process(sample.getPcmValue(), setting);
        sample.setPcmValue(processed);
        sample.addEffect(typeid(*effect).name());  // logs effect name
    }
    }catch(...){
        sample.setPcmValue(pcmValue);  // optional: reset to original
    }
}
