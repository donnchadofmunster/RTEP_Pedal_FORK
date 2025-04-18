#ifndef EFFECT_H
#define EFFECT_H

#include <any>
#include <string>
#include "Config.h"

/**
 * @class Effect
 * @brief Abstract base class for all audio effects.
 *
 * This interface defines the contract for DSP effects used in the signal chain.
 * Each concrete effect must implement the `process()` method, which modifies
 * a sample according to its internal state (setting and activation flag).
 */
class Effect
{
public:
    virtual ~Effect() = default;

    /**
     * @brief Processes a single audio sample with the effect.
     * @param sample The input PCM sample value (typically between -1.0 and 1.0).
     * @return The processed output sample.
     */
    virtual float process(float sample) = 0;

    /**
     * @brief Configures the effect from global configuration.
     *        Will be called once during initialisation or when config is updated.
     */
    void configure(const Config &config)
    {
        parseConfig(config);
    }

    /**
     * @brief Sets the effect's active status.
     */
    void setActive(bool active)
    {
        IsActive = active;
    }

    /**
     * @brief Checks whether the effect is currently active.
     */
    bool isActive() const
    {
        return IsActive;
    }

    /**
     * @brief Sets the internal configuration value for the effect.
     * @param value The new configuration value as std::any.
     */
    void setSetting(const std::any &value)
    {
        Setting = value;
    }

protected:
    /**
     * @brief Subclass-implemented config parser.
     */
    virtual void parseConfig(const Config &config) = 0;

    bool IsActive = true; ///< Whether the effect should be applied.
    std::any Setting;     ///< Stored parameter value (e.g. gain, pitch, threshold).
};

#endif // EFFECT_H
