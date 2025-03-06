#include "Harmonizer.h"

float Harmonizer::process(float &sample)
{
    sample *= 1.5f;

    return sample; // Return the processed sample
}
