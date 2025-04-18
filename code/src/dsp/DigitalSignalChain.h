#ifndef DIGITALSIGNALCHAIN_H
#define DIGITALSIGNALCHAIN_H

#include <atomic>
#include <string>
#include <memory>
#include "Effect.h"
#include "Sample.h"

constexpr size_t MAX_EFFECTS = 4; ///< Max number of effects in a signal chain

/**
 * @class DigitalSignalChain
 * @brief Manages a hot-swappable chain of real-time-safe audio effects.
 *
 * This class supports atomic switching between two internal effect chains,
 * allowing effects to be loaded from a file and swapped in with no locking
 * or disruption to the audio processing loop.
 */
class DigitalSignalChain
{
public:
    /**
     * @brief Constructor. Initializes effect chains.
     */
    DigitalSignalChain();

    /**
     * @brief Applies the currently active chain of effects to a sample.
     * @param sample The audio sample to be processed.
     */
    void applyEffects(Sample &sample);

    /**
     * @brief Updates all effects given a Config object
     * @param config The Configuration object to be used.
     */
    void configureEffects(Config &config);

private:
    /**
     * @brief Registers all effects from factory
     */
    void registerAllEffects();
    struct EffectSlot
    {
        std::shared_ptr<Effect> effect; ///< shared pointer to an effect (shared ownership lives elsewhere)
        char name[64] = {0};      ///< Name of the effect for logging
    };

    struct Chain
    {
        EffectSlot effects[MAX_EFFECTS]; ///< Fixed array of effect slots
        size_t count = 0;                ///< Number of active effects
    };

    Chain chains[2];                      ///< Double buffer for hot-swapping
    std::atomic<size_t> activeChainIndex; ///< Active chain index for lock-free switching
};

#endif // DIGITALSIGNALCHAIN_H
