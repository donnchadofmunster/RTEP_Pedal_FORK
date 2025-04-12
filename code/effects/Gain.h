#pragma once
#include "Effect.h"

class Gain : public Effect {
public:
    Gain();
    float process(float sample, float setting) override;
    ~Gain();
};