#include "Sample.h"
#include <iostream>

Sample::Sample(float inValue, double timeIndex)
{
    //std::cout << "[Sample] Initilialising with value: " << inValue << "\n";
    this->pcmValue = inValue;
    this->timeIndex = timeIndex;
    //std::cout << "[Sample] Initilialised with value: " << this->pcmValue << "\n";
}

float Sample::getPcmValue() const
{
    return pcmValue;
}

void Sample::setPcmValue(float value)
{
    pcmValue = value;
}

double Sample::getTimeIndex() const
{
    return timeIndex;
}

void Sample::addEffect(const std::string &effectName)
{
    appliedEffects.push_back(effectName);
}

const std::vector<std::string> &Sample::getAppliedEffects() const
{
    return appliedEffects;
}
