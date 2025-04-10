#include "OctaveDoubler.h"
#include "EffectRegistration.h"
#include <cmath>

float OctaveDoubler::process(float &sample) {
    return std::abs(sample);  // full-wave rectification
}

REGISTER_EFFECT(OctaveDoubler)
