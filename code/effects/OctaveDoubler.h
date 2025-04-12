#pragma once
#include "Effect.h"

class OctaveDoubler : public Effect {
public:
    OctaveDoubler();
    float process(float sample, float setting) override;
    ~OctaveDoubler();

private:
    int lastSign;
};
