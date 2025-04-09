#include "Harmonizer.h"
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <thread>

int main() {
    Harmonizer harmonizer("input2.wav", "output.wav", {12, 3, 4, 2});
    harmonizer.createChord();
    return 0;
}
