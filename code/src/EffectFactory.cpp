#include "EffectFactory.h"
#include <iostream>
#include <cxxabi.h>

EffectFactory& EffectFactory::instance() {
    static EffectFactory* factory = new EffectFactory();
    return *factory;
}


void EffectFactory::registerEffect(const std::string& name, Creator creator) {
    std::cout << "[EffectFactory] Registered: " << name << std::endl;

    // Add guard for re-registration
    if (registry.find(name) != registry.end()) {
        std::cerr << "[EffectFactory] Warning: Effect '" << name << "' is already registered. Overwriting.\n";
    }

    registry[name] = creator;
}


std::shared_ptr<Effect> EffectFactory::createEffect(const std::string& name) const {
    auto it = registry.find(name);
    if (it != registry.end()) {
        std::cout << "[EffectFactory] Creating effect: " << name << std::endl;
        auto effect = it->second();
        int status;
        char* realname = abi::__cxa_demangle(typeid(*effect).name(), 0, 0, &status);
        std::cout << "[EffectFactory] Created effect: " << (status == 0 ? realname : typeid(*effect).name()) << std::endl;
        free(realname);
        return effect; 
    }

    std::cerr << "[EffectFactory] Effect not found: " << name << std::endl;
    return nullptr;
}

