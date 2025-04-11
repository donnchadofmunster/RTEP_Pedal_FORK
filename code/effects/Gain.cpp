#include "Gain.h"
#include "EffectRegistration.h"
#include <cmath>
#include <iostream>

Gain::Gain() {} 

float Gain::process(float sample){
    const float gain = 0.01f;
    return sample*gain;
}

Gain::~Gain() {
    std::cout << "[Gain] Gain destroyed cleanly\n";
}

REGISTER_EFFECT_AUTO(Gain);
