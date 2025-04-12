#pragma once
#include "Effect.h"

class Fuzz : public Effect {
public:
    Fuzz();
    float process(float sample, float setting) override;
    ~Fuzz();
};