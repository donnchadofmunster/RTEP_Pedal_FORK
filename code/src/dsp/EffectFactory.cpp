#include "EffectFactory.h"
#include <iostream>

EffectFactory& EffectFactory::instance() {
    static EffectFactory* factory = new EffectFactory();
    return *factory;
}

void EffectFactory::registerEffect(const std::string& name, Creator creator) {
    std::cout << "[EffectFactory] Registered: " << name << std::endl;

    if (registry.find(name) != registry.end()) {
        std::cerr << "[EffectFactory] Warning: Effect '" << name << "' is already registered. Overwriting.\n";
    }

    registry[name] = creator;
}

std::vector<std::string> EffectFactory::getAllRegisteredEffectNames() const
{
    std::vector<std::string> names;
    for (const auto &entry : registry)
    {
        names.push_back(entry.first);
    }
    return names;
}

std::shared_ptr<Effect> EffectFactory::createEffect(const std::string& name) const {
    auto it = registry.find(name);
    if (it != registry.end()) {
        std::cout << "[EffectFactory] Creating effect: " << name << std::endl;
        auto effect = it->second();

        std::cout << "[EffectFactory] Created effect: " << typeid(*effect).name() << std::endl;

        return effect;
    }

    std::cerr << "[EffectFactory] Effect not found: " << name << std::endl;
    return nullptr;
}
