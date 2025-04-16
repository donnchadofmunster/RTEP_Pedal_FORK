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
     * @param pcmValue The PCM value of the sample.
     * @param timeIndex The time index of the sample in the audio stream.
     */
    Sample(float pcmValue, double timeIndex);

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
     * @brief Gets the time index of the sample.
     * @return The time index.
     */
    double getTimeIndex() const;

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
    int16_t  pcmValue;                          ///< The PCM value of the sample.
    double timeIndex;                        ///< The time index of the sample.
    std::vector<std::string> appliedEffects; ///< List of applied effects.
};

#endif // SAMPLE_H  