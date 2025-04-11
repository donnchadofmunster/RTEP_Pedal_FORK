#include "Harmonizer.h"
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <thread>

int main() {
    Harmonizer harmonizer("input_440.wav", "output.wav", {3});
    harmonizer.createChord();
    return 0;
}