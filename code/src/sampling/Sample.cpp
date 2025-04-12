#include "Sample.h"

Sample::Sample(float pcmValue, double timeIndex) : pcmValue(pcmValue), timeIndex(timeIndex) {}

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
