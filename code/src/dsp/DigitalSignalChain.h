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
     * @brief Loads a chain of effects from a text file.
     * @param filepath Path to the effect configuration file.
     * @return True if loading succeeded, false otherwise.
     */
    bool loadEffectsFromFile(const std::string &filepath);

    /**
     * @brief Applies the currently active chain of effects to a sample.
     * @param sample The audio sample to be processed.
     * @param setting A global control value passed to each effect.
     */
    void applyEffects(Sample &sample, float setting);

private:
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
