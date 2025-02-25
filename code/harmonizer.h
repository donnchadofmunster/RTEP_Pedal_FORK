#ifndef HARMONIZER_H
#define HARMONIZER_H
#include "Effect.h"

class Harmonizer : public Effect
{
public:
    void process(float &sample) override;
};
#endif // HARMONIZER_H