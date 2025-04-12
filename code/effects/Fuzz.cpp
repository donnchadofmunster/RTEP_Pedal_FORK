#include "Fuzz.h"
#include "EffectRegistration.h"
#include <cmath>
#include <iostream>

Fuzz::Fuzz() {} 

float Fuzz::process(float sample, float setting = 3.0){
    const float threshold = 0.01f * setting;
    if (sample > threshold) return threshold;
    if (sample < -threshold) return -threshold;
    return sample;
}

Fuzz::~Fuzz() {
    std::cout << "[Fuzz] Fuzz destroyed cleanly\n";
}

REGISTER_EFFECT_AUTO(Fuzz);
