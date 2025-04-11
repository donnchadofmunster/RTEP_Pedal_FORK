// ForceEffects.cpp
#include <iostream>

extern "C" void ForceLink_OctaveDoubler_Effect();

void ForceAllEffects() {
    std::cout << "[ForceEffects] Linking OctaveDoubler\n";
    ForceLink_OctaveDoubler_Effect();
    // Future effects just get auto-added here (or auto-discovered in a generator)
}
