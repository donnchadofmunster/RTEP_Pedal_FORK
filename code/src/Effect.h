#ifndef EFFECT_H
#define EFFECT_H

class Effect
{
public:
    virtual ~Effect() = default;
    virtual float process(float sample, float setting) = 0;
};
#endif // EFFECT_H