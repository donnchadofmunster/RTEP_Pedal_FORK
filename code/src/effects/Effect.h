#ifndef EFFECT_H
#define EFFECT_H

/**
 * @class Effect
 * @brief Abstract base class for all audio effects.
 *
 * This interface defines the contract for all DSP effects used in the signal chain.
 * Every concrete effect must implement the `process()` method, which modifies a sample
 * according to the effect's logic and an optional control parameter (e.g., gain, pitch, etc.).
 */
class Effect
{
public:
    /**
     * @brief Virtual destructor for safe polymorphic destruction.
     */
    virtual ~Effect() = default;

    /**
     * @brief Processes a single audio sample with the effect.
     * @param sample The input PCM sample value (typically between -1.0 and 1.0).
     * @param setting An effect-specific control value (e.g., gain level, pitch shift).
     * @return The processed output sample.
     */
    virtual float process(float sample, float setting) = 0;
};

#endif // EFFECT_H
