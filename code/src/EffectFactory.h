#pragma once
#include <string>
#include <functional>
#include <map>
#include <memory>
#include "Effect.h"

class EffectFactory {
public:
    using Creator = std::function<std::shared_ptr<Effect>()>;

    static EffectFactory& instance();

    void registerEffect(const std::string& name, Creator creator);
    std::shared_ptr<Effect> createEffect(const std::string& name) const;

private:
    std::map<std::string, Creator> registry;
};
