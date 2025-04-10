#pragma once
#include "Effect.h"
#include <cmath>

class OctaveDoubler : public Effect {
public:
    float process(float &sample) override;
    virtual ~OctaveDoubler() = default;
};
