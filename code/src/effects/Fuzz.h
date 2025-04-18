#pragma once
#include "Effect.h"

class Fuzz : public Effect {
public:
    Fuzz();
    float process(float sample) override;
    ~Fuzz();

protected:
    void parseConfig(const Config &config) override;
};