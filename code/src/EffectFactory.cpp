#include "EffectFactory.h"

EffectFactory& EffectFactory::instance() {
    static EffectFactory factory;
    return factory;
}

void EffectFactory::registerEffect(const std::string& name, Creator creator) {
    registry[name] = creator;
}

std::shared_ptr<Effect> EffectFactory::createEffect(const std::string& name) const {
    auto it = registry.find(name);
    if (it != registry.end()) {
        return it->second();
    }
    return nullptr;
}
