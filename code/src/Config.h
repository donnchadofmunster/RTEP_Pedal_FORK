#ifndef CONFIG_H
#define CONFIG_H
#include <map>
#include <memory>
#include <string>
#include "Effect.h"

class Config
{
public:
    using EffectType = std::string;
    std::map<std::string, float> parameters;
    std::shared_ptr<Effect> getEffect();
};
#endif // CONFIG_H