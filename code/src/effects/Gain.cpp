#include "Gain.h"
#include "EffectRegistration.h"
#include <iostream>
#include <cmath>

Gain::Gain()
{
    IsActive = true;  // Default active
    Setting = 100.0f; // Default gain percentage (i.e., 100%)
}

float Gain::process(float sample)
{
    if (!IsActive){
        //std::cout << "[Gain] Skipping process()\n";
        return sample;
    }
        

    try
    {
        //std::cout << "[Gain] Applying Gain\n";
        float gain = std::any_cast<float>(Setting);
        return sample * (0.01f * gain);
    }
    catch (const std::bad_any_cast &)
    {
        //std::cerr << "[Gain] Invalid setting type\n";
        return sample;
    }
}

void Gain::parseConfig(const Config &config)
{
    //std::cout << "[Gain] Reconfigured Gain... \n";
    IsActive = config.contains("gain");
    //std::cout << "[Gain] IsActive: ";
    //std::cout << IsActive;
    Setting = config.get<float>("gain", 100.0f); // Default to 100% gain

}

Gain::~Gain()
{
    std::cout << "[Gain] Gain destroyed cleanly\n";
}

REGISTER_EFFECT_AUTO(Gain);
