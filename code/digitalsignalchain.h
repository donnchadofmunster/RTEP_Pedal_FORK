#ifndef DIGITALSIGNALCHAIN_H
#define DIGITALSIGNALCHAIN_H
#include "Effect.h"
#include <vector>
#include <memory>

class DigitalSignalChain
{
    std::vector<std::shared_ptr<Effect>> effects;

public:
    void registerEffect(std::shared_ptr<Effect> effect);
    void applyEffects(float &sample);
};
#endif // DIGITALSIGNALCHAIN_H