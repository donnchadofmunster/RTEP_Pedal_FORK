#include "Sample.h"
#include <iostream>

Sample::Sample(float inValue)
{
    //std::cout << "[Sample] Initilialising with value: " << inValue << "\n";
    this->pcmValue = inValue;
}

float Sample::getPcmValue() const
{
    return pcmValue;
}

void Sample::setPcmValue(float value)
{
    pcmValue = value;
}

void Sample::addEffect(const std::string &effectName)
{
    appliedEffects.push_back(effectName);
}

const std::vector<std::string> &Sample::getAppliedEffects() const
{
    return appliedEffects;
}
