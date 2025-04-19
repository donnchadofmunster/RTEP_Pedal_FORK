#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
#include <string>

/**
 * @class Sample
 * @brief Represents a PCM sample, its timestamp, and a history of applied effects.
 */
class Sample
{
public:
    /**
     * @brief Constructor for Sample.
     * @param inValue The PCM value of the sample.
     */
    Sample(float inValue);

    /**
     * @brief Gets the PCM value of the sample.
     * @return The PCM value.
     */
    float getPcmValue() const;

    /**
     * @brief Sets the PCM value of the sample.
     * @param value The new PCM value.
     */
    void setPcmValue(float value);

    /**
     * @brief Adds an effect name to the list of applied effects.
     * @param effectName The name of the applied effect.
     */
    void addEffect(const std::string &effectName);

    /**
     * @brief Gets the list of applied effects.
     * @return A vector of effect names.
     */
    const std::vector<std::string> &getAppliedEffects() const;

private:
    float  pcmValue;                          ///< The PCM value of the sample.
    std::vector<std::string> appliedEffects; ///< List of applied effects.
};

#endif // SAMPLE_H  