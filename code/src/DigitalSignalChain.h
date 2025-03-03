#ifndef DIGITALSIGNALCHAIN_H
#define DIGITALSIGNALCHAIN_H

#include <vector>
#include <memory>
#include "Effect.h"

/**
 * @class DigitalSignalChain
 * @brief Manages a chain of audio effects and applies them sequentially to a sample.
 */
class DigitalSignalChain
{
public:
    /**
     * @brief Registers an effect to be applied in the processing chain.
     * @param effect A shared pointer to an Effect instance.
     */
    void registerEffect(std::shared_ptr<Effect> effect);

    /**
     * @brief Applies all registered effects to a sample.
     * @param sample The input sample to process.
     * @return The processed sample after all effects are applied.
     */
    float applyEffects(float sample);

private:
    std::vector<std::shared_ptr<Effect>> effects; ///< List of registered effects.
};

#endif