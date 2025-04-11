#include "Fuzz.h"
#include "EffectRegistration.h"
#include <cmath>
#include <iostream>

Fuzz::Fuzz() {} 

float Fuzz::process(float sample){
    const float threshold = 0.01f;
    if (sample > threshold) return threshold;
    if (sample < -threshold) return -threshold;
    return sample;
}

Fuzz::~Fuzz() {
    std::cout << "[Fuzz] Fuzz destroyed cleanly\n";
}

REGISTER_EFFECT_AUTO(Fuzz);
