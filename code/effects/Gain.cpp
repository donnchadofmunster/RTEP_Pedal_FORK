#include "Gain.h"
#include "EffectRegistration.h"
#include <cmath>
#include <iostream>

Gain::Gain() {} 

float Gain::process(float sample, float setting = 3.0){
    const float gain = 0.01f * setting;
    return sample*gain;
}

Gain::~Gain() {
    std::cout << "[Gain] Gain destroyed cleanly\n";
}

REGISTER_EFFECT_AUTO(Gain);
