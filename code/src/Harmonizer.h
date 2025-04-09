#ifndef HARMONIZER_H
#define HARMONIZER_H

#include <string>
#include <vector> // Ensure vector is included
#include <sndfile.h>
#include "../lib/signalsmith-stretch/signalsmith-stretch.h"
#include "../lib/signalsmith-stretch/cmd/util/stopwatch.h"
#include "../lib/signalsmith-stretch/cmd/util/memory-tracker.h"
#include "../lib/signalsmith-stretch/cmd/util/wav.h"

class Harmonizer {
public:
    // Constructor: Takes in Input, Output, and the Semitone values intended to pitch shift by
    Harmonizer(const std::string& inputWav = "input.wav", 
               const std::string& outputWav = "output.wav", 
               const std::vector<int>& semitones = {0});

    // Updates the inputs so new values can be used without creating a new object
    void updateInputs(const std::string& inputWav = "input.wav", 
                      const std::string& outputWav = "output.wav", 
                      const std::vector<int>& semitones = {0});
    
    // Applies pitch shifting and creates a new output
    bool process(int iteration = 0);

    // Creates a chord by pitch shifting and stacking the Wav files
    bool createChord();

private:
    std::string inputWav;
    std::string outputWav;
    std::vector<int> semitones; // Change this to a regular vector, not a reference
    int currentSemitone = 0; // The current semitone value to be pitch shifted by
    double tonality = 8000; // Sets tonality to 8000hz
    double time = 1; // Used to stretch audio, 1 is no stretch
    bool exactLength = false; // If the exact length is parsed in, set to true

    Wav inWav;
    Wav outWav;
    signalsmith::stretch::SignalsmithStretch<float> stretch;
    signalsmith::MemoryTracker initMemory;
    signalsmith::MemoryTracker processMemory;
    signalsmith::Stopwatch stopwatch;

    void setupStretch(int currentSemitone);
    void processAudio();
    void reportMemoryUsage();
    void reportProcessingStats(double processSeconds, double processRate, double processPercent);
};

#endif // HARMONIZER_H
