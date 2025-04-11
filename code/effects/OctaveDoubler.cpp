#include "OctaveDoubler.h"
#include "EffectRegistration.h"
#include <cmath>
#include <iostream>

float OctaveDoubler::process(float sample) {
    return std::abs(sample);  // full-wave rectification
}

OctaveDoubler::~OctaveDoubler() {
    std::cout << "[OctaveDoubler] OctaveDoubler destroyed cleanly\n";
}

// Automatically register the effect during static initialization
REGISTER_EFFECT_AUTO(OctaveDoubler);
