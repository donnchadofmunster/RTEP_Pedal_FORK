#include "Config.h"
#include "Harmonizer.h"

std::shared_ptr<Effect> Config::getEffect()
{
    return std::make_shared<Harmonizer>();
}
