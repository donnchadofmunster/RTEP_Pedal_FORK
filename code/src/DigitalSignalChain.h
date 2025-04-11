#ifndef DIGITALSIGNALCHAIN_H
#define DIGITALSIGNALCHAIN_H

#include <vector>
#include <memory>
#include "Effect.h"
#include "Sample.h"
#include <string>

/**
 * @class DigitalSignalChain
 * @brief Manages a chain of audio effects and applies them sequentially to a Sample object.
 */
class DigitalSignalChain
{
public:


    bool loadEffectsFromFile(const std::string& filepath); // Loads effects from file
    /**
     * @brief Registers an effect to be applied in the processing chain.
     * @param effect A shared pointer to an Effect instance.
     */
    void registerEffect(std::shared_ptr<Effect> effect);

    /**
     * @brief Applies all registered effects to a Sample object.
     * @param sample The input Sample to process.
     */
    void applyEffects(Sample &sample);

private:

    std::vector<std::shared_ptr<Effect>> effects; ///< List of registered effects.

};

#endif // DIGITALSIGNALCHAIN_H