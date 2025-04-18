#pragma once
#include "Effect.h"

/**
 * @class Gain
 * @brief A basic gain control effect that multiplies the input by a gain factor.
 */
class Gain : public Effect
{
public:
    Gain();
    float process(float sample) override;
    ~Gain();

protected:
    void parseConfig(const Config &config) override;
};
