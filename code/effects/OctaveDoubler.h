#pragma once
#include "Effect.h"

class OctaveDoubler : public Effect {
public:
    OctaveDoubler();
    float process(float sample) override;
    ~OctaveDoubler();

private:
    int lastSign;
};
