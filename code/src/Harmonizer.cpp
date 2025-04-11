#include "Harmonizer.h"
#include <iostream>
#include <cmath>
#include <filesystem>

// Constructor
Harmonizer::Harmonizer(const std::string& inputWav, const std::string& outputWav, const std::vector<int>& semitones)
    : inputWav(inputWav), outputWav(outputWav), semitones(semitones) 
{
    std::filesystem::path assetsPath = "assets";
    this->inputWav = (assetsPath / inputWav).string();  
    this->outputWav = (assetsPath / outputWav).string();
}

void Harmonizer::updateInputs(const std::string& inputWav, const std::string& outputWav, const std::vector<int>& semitones) {
    this->inputWav = inputWav;
    this->outputWav = outputWav;
    this->semitones = semitones;
}

void Harmonizer::setupStretch(int currentSemitone) {
    if (!inWav.read(inputWav).warn()) { // If it can't read the file...
        std::cerr << "Error reading input WAV file!" << std::endl;
        return;
    }
    
    size_t inputLength = inWav.samples.size() / inWav.channels;
    outWav.channels = inWav.channels;
    outWav.sampleRate = inWav.sampleRate;
    
    int outputLength = std::round(inputLength * time);

    stretch.presetDefault(inWav.channels, inWav.sampleRate);
    stretch.setTransposeSemitones(currentSemitone, tonality / inWav.sampleRate);

    size_t paddedInputLength = inputLength + stretch.inputLatency();
    inWav.samples.resize(paddedInputLength * inWav.channels);

    int tailSamples = exactLength ? stretch.outputLatency() : (stretch.outputLatency() + stretch.inputLatency());
    int paddedOutputLength = outputLength + tailSamples;
    outWav.samples.resize(paddedOutputLength * outWav.channels);
}

void Harmonizer::processAudio() {
    size_t inputLength = inWav.samples.size() / inWav.channels;
    size_t outputLength = std::round(inputLength * time);

    stretch.seek(inWav, stretch.inputLatency(), 1 / time);
    inWav.offset += stretch.inputLatency();
    
    stretch.process(inWav, inputLength, outWav, outputLength);
    outWav.offset += outputLength;
    stretch.flush(outWav, stretch.outputLatency());
    outWav.offset -= outputLength;
}

void Harmonizer::reportMemoryUsage() {
    if (processMemory.implemented) {
        std::cout << "\tallocated " << (processMemory.allocBytes / 1000) << "kB, freed " << (processMemory.freeBytes / 1000) << "kB\n";
    }
}

void Harmonizer::reportProcessingStats(double processSeconds, double processRate, double processPercent) {
    std::cout << "Process:\n\t" << processSeconds << "s, " << processRate << "x realtime, " << processPercent << "% CPU\n";
    reportMemoryUsage();
}

bool Harmonizer::process(int iteration) {
    setupStretch(semitones[iteration]); // Setup
    printf("Processing %s with %d semitones\n", inputWav.c_str(), semitones[iteration]); // Prints the current file being processed
    processAudio();

    std::filesystem::path assetsPath = "assets";
    std::string outputFileName = "output" + std::to_string(iteration) + ".wav";
    this->outputWav = (assetsPath / outputFileName).string();
    outWav.write(outputWav); // Creates the new .Wav file

    return true;
}

std::string Harmonizer::mergeWavs(const char* infilename, const char* infilename2, const char* outfilename) {
    static double data [1024] ;
    static double data2 [1024] ;
    static double outdata [1024] ;
    SNDFILE *infile, *outfile, *infile2 ;
    SF_INFO sfinfo ;
    int readcount ;
    SF_INFO sfinfo2 ;
    int readcount2 ;

    infile = sf_open (infilename, SFM_READ, &sfinfo);
    infile2 = sf_open (infilename2, SFM_READ, &sfinfo2);
    outfile = sf_open (outfilename, SFM_WRITE, &sfinfo);

    while ((readcount = sf_read_double (infile, data, 1024)) && (readcount2 = sf_read_double (infile2, data2, 1024))) { 
        data_processing (data, readcount, sfinfo.channels) ;
        data_processing(data2, readcount2, sfinfo2.channels) ;
        for (int j = 0; j < 1024; ++j) {
            outdata[j] = (data[j] + data2[j]) - (data[j] * data2[j]) / 65535;
        }
        sf_write_double (outfile, outdata , readcount) ;
    };
    std::filesystem::remove(infilename);
    std::filesystem::remove(infilename2);

    sf_close(infile);
    sf_close(infile2);
    sf_close(outfile);
    
    return outfilename;
}

bool Harmonizer::createChord() {
    stopwatch.start(); // Used to report on the time taken for pitch shifting, useful for testing
    std::filesystem::path assetsPath = "assets";

    // Create the temporary pitch shifted WAV files
    for (size_t i = 0; i < semitones.size(); ++i) {
        process(i);
    }

    if (semitones.size() > 1) {
        std::string output = mergeWavs(
            (assetsPath / "output0.wav").string().c_str(),
            (assetsPath / "output1.wav").string().c_str(),
            (assetsPath / "mergedOutput0.wav").string().c_str()
        );

        std::string last_output = output;

        for (size_t i = 2; i < semitones.size(); ++i) {
            std::string inputFilePath1 = (assetsPath / ("output" + std::to_string(i) + ".wav")).string();
            std::string outputFilePath = (assetsPath / ("mergedOutput" + std::to_string(i) + ".wav")).string();
            output = mergeWavs(inputFilePath1.c_str(), last_output.c_str(), outputFilePath.c_str());
            last_output = output;
        }
        std::filesystem::rename(last_output, assetsPath / "output.wav");
    } else {
        std::filesystem::rename(assetsPath / "output0.wav", assetsPath / "output.wav");
    }
    
    double processSeconds = stopwatch.seconds(stopwatch.lap()); // Shows how long it took to process the audio
    double processRate = (inWav.length() / inWav.sampleRate) / processSeconds; // Shows how fast pitch shifting is compared to length of input
    double processPercent = 100 / processRate; // Shows the CPU usage
    reportProcessingStats(processSeconds, processRate, processPercent); // Displays the stats of the pitch shifting
    return true;

}

void Harmonizer::data_processing(double *data, int count, int channels) { 
    double channel_gain [6] = { 1, 1, 1, 1, 1, 1 } ;
    int k, chan ;
  
    for (chan = 0 ; chan < channels ; chan ++)
      for (k = chan ; k < count ; k+= channels)
        data [k] *= channel_gain [chan] ;
  
    return ;
}