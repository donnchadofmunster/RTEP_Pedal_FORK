#include "OctaveDoubler.h"
#include "EffectRegistration.h"
#include <cmath>
#include <iostream>

OctaveDoubler::OctaveDoubler() : lastSign(1) {}

float OctaveDoubler::process(float sample) {
    // Flip the waveform every time the signal crosses zero (very primitive doubling)
    int currentSign = (sample >= 0.0f) ? 1 : -1;
    float doubledSample = (currentSign != lastSign) ? -sample : sample;
    lastSign = currentSign;
    return doubledSample;
}

OctaveDoubler::~OctaveDoubler() {
    std::cout << "[OctaveDoubler] OctaveDoubler destroyed cleanly\n";
}

REGISTER_EFFECT_AUTO(OctaveDoubler);
