#ifndef UIHANDLER_H
#define UIHANDLER_H

#include "Display.h"
#include "../options/Config.h"
#include "../gpio_event/gpioevent.h"
#include "DigitalSignalChain.h"
#include <string>
#include <vector>
#include <array>
#include <map>

// Forward declarations
class EncoderHandler;

class UIHandler{
public:
    // Singleton pattern
    static UIHandler& getInstance();

    // Initialize UI subsystems
    bool init(DigitalSignalChain &dspChain);

    // Update display based on current state
    void update();

    // Process encoder events
    void handleEncoder(int encoderID, int action);

    // Encoder action types
    enum EncoderAction {
        ACTION_LEFT = 0,
        ACTION_RIGHT = 1,
        ACTION_PUSH = 2
    };

    // Encoder IDs
    enum EncoderID {
        ENC_CURSOR = 0,      // Controls cursor position on piano keyboard
        ENC_EFFECT_SELECT = 1,     // Controls selected effect and push to toggle (
        ENC_EFFECT_EDIT = 2,     // Controls selected effect value 

    };

private:
    // Private constructor for singleton
    DigitalSignalChain *dspChain;
    UIHandler();
    ~UIHandler();
    
    // Prevent copies
    UIHandler(const UIHandler&) = delete;
    UIHandler& operator=(const UIHandler&) = delete;

    static constexpr int MIN_CURSOR_VALUE = -17;
    static constexpr int MAX_CURSOR_VALUE = 17;

    // Effect parameter definition
    struct EffectParam {
        std::string name;        // Effect name
        std::string configKey;   // Key in config.cfg
        bool isEnabled;          // Whether effect is currently enabled
        
        // For numeric parameters (float or int)
        float minValue;          // Minimum value
        float maxValue;          // Maximum value
        float stepSize;          // Step size for encoder increments
        float currentValue;      // Current value
        
        // For harmonizer (array of semitones)
        static const int MAX_SEMITONES = 8;
        std::array<int, MAX_SEMITONES> semitones;
        int semitoneCount;
        
        // Type of parameter
        enum ParamType { TYPE_FLOAT, TYPE_INT, TYPE_SEMITONES } type;
        
        // Existing constructor remains unchanged
        EffectParam(const std::string& n, const std::string& key, 
                    ParamType t, float min, float max, float step) 
            : name(n), configKey(key), isEnabled(false), 
            minValue(min), maxValue(max), stepSize(step), 
            currentValue(0), semitoneCount(0), type(t) {}

        // Add overloaded constructor for 8-bit resolution
        EffectParam(const std::string& n, const std::string& key, 
                    ParamType t, float min, float max) 
                : name(n), configKey(key), isEnabled(false), 
                minValue(min), maxValue(max), 
                stepSize((max - min) / 255.0f), // 8-bit resolution
                currentValue(0), semitoneCount(0), type(t) {}

    };
    
    // Handle specific encoder actions
    void handleCursorEncoder(EncoderAction action);
    void handleEffectSelectEncoder(EncoderAction action);
    void handleEffectEditEncoder(EncoderAction action);
    
    // Helper methods for semitone selection
    bool isSemitoneSelected(int note) const;
    void toggleSemitone(int note);
    
    // Update config based on the current UI state
    void updateConfig();
    
    // Load current settings from config
    void loadFromConfig();
    
    // Helper for converting semitones array to string
    std::string semitonesToString(const std::array<int, EffectParam::MAX_SEMITONES>& semitones, int count);
    
    // Helper for parsing semitones string to array
    void parseSemitonesString(const std::string& str, std::array<int, EffectParam::MAX_SEMITONES>& semitones, int& count);

    // Display object
    Display display;
    
    // Config reference
    Config& config;
    
    // Current cursor position on keyboard
    int cursorPosition;
    
    // Effect parameters
    std::vector<EffectParam> effects;
    
    // Currently selected effect for display
    int currentEffectIndex;
    

};

#endif // UIHANDLER_H