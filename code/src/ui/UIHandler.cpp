#include "UIHandler.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>


// For timestamp generation
using namespace std::chrono;

// Static singleton instance
UIHandler& UIHandler::getInstance() {
    static UIHandler instance;
    return instance;
}

UIHandler::UIHandler() 
    : config(Config::getInstance()),
      cursorPosition(0),
      currentEffectIndex(1) {  // Start with fuzz selected
    
    // Initialize with 8-bit resolution
    effects.emplace_back("Harmonizer", "harmonizer", EffectParam::TYPE_SEMITONES, 0.0f, 0.0f);
    effects.emplace_back("Fuzz", "fuzz", EffectParam::TYPE_FLOAT, 1.0f, 6.0f);
    effects.emplace_back("Gain", "gain", EffectParam::TYPE_FLOAT, 0.0f, 200.0f);
    
}

UIHandler::~UIHandler() {
    // Cleanup resources if needed
}

bool UIHandler::init() {
    // Initialize the display
    if (!display.init()) {
        std::cerr << "Failed to initialize display" << std::endl;
        return false;
    }
    
    // Load initial settings from config
    loadFromConfig();
    
    // Update display with initial state
    update();
    
    return true;
}

void UIHandler::update() {
    // Set cursor position on piano keyboard
    display.setCursor(cursorPosition);
    
    // Set selected notes for harmonizer
    if (effects[0].isEnabled && effects[0].semitoneCount > 0) {
        display.setSelectedNotes(effects[0].semitones.data(), effects[0].semitoneCount);
    } else {
        display.setSelectedNotes(nullptr, 0);
    }
    
    // Get the currently selected effect (Fuzz or Gain)
    const std::string& effectName = effects[currentEffectIndex].name;
    float effectValue = effects[currentEffectIndex].currentValue;
    bool isEnabled = effects[currentEffectIndex].isEnabled;
    
    // Update the display with current effect information
    display.update(effectName.c_str(), effectValue, isEnabled);
}

void UIHandler::handleEncoder(int encoderID, int action) {
    switch (encoderID) {
        case ENC_CURSOR:
            handleCursorEncoder(static_cast<EncoderAction>(action));
            break;
            
        case ENC_EFFECT_SELECT:
            handleEffectSelectEncoder(static_cast<EncoderAction>(action));
            break;
            
        case ENC_EFFECT_EDIT:
            handleEffectEditEncoder(static_cast<EncoderAction>(action));
            break;
            
        default:
            return;
    }
    
    // Update config with any changes
    updateConfig();
    
    // Refresh display
    update();
}

void UIHandler::handleCursorEncoder(EncoderAction action) {
    if (action == ACTION_LEFT) {
        // Move cursor left
        cursorPosition--;
        if (cursorPosition < MIN_CURSOR_VALUE) {
            cursorPosition = MAX_CURSOR_VALUE; // Wrap around
        }
    } 
    else if (action == ACTION_RIGHT) {
        // Move cursor right
        cursorPosition++;
        if (cursorPosition > MAX_CURSOR_VALUE) {
            cursorPosition = MIN_CURSOR_VALUE; // Wrap around
        }
    } 
    else if (action == ACTION_PUSH) {
        // Simple toggle of the note if harmonizer is enabled
        if (effects[0].isEnabled) {
            toggleSemitone(cursorPosition);
        }
    }
}

void UIHandler::handleEffectSelectEncoder(EncoderAction action) {
    // We only have two effects - fuzz (index 1) and gain (index 2)
    
    if (action == ACTION_LEFT || action == ACTION_RIGHT) {
        // Toggle between fuzz and gain
        if (currentEffectIndex == 1) {
            currentEffectIndex = 2;  // Switch from fuzz to gain
        } else {
            currentEffectIndex = 1;  // Switch from gain to fuzz
        }
    } 
    else if (action == ACTION_PUSH) {
        // Toggle the current effect on/off
        effects[currentEffectIndex].isEnabled = !effects[currentEffectIndex].isEnabled;
    }
}

void UIHandler::handleEffectEditEncoder(EncoderAction action) {
    // Only adjust parameters if we have a selected effect (should be 1 or 2)
    if (currentEffectIndex < 1 || currentEffectIndex >= static_cast<int>(effects.size())) {
        return;
    }
    
    auto& effect = effects[currentEffectIndex];
    
    // Only adjust enabled effects
    if (!effect.isEnabled) {
        return;
    }
    
    if (action == ACTION_LEFT || action == ACTION_RIGHT) {
        float delta = (action == ACTION_LEFT) ? -effect.stepSize : effect.stepSize;
        
        // Adjust parameter value
        effect.currentValue += delta;
        
        // Clamp to min/max
        if (effect.currentValue < effect.minValue) effect.currentValue = effect.minValue;
        if (effect.currentValue > effect.maxValue) effect.currentValue = effect.maxValue;
        
        // Round to integer if needed
        if (effect.type == EffectParam::TYPE_INT) {
            effect.currentValue = std::round(effect.currentValue);
        }
    }
    else if (action == ACTION_PUSH) {
        // Could implement a "reset to default" feature here if desired
        // For now, does nothing
    }
}

bool UIHandler::isSemitoneSelected(int note) const {
    const auto& semitones = effects[0].semitones;
    int count = effects[0].semitoneCount;
    
    for (int i = 0; i < count; i++) {
        if (semitones[i] == note) {
            return true;
        }
    }
    return false;
}

void UIHandler::toggleSemitone(int note) {
    auto& effect = effects[0]; // Harmonizer effect
    
    // Check if note is already selected
    for (int i = 0; i < effect.semitoneCount; i++) {
        if (effect.semitones[i] == note) {
            // Remove this semitone by shifting all elements after it
            for (int j = i; j < effect.semitoneCount - 1; j++) {
                effect.semitones[j] = effect.semitones[j + 1];
            }
            effect.semitoneCount--;
            return;
        }
    }
    
    // Note not found, add it if we have room
    if (effect.semitoneCount < EffectParam::MAX_SEMITONES) {
        effect.semitones[effect.semitoneCount++] = note;
        
        // Optional: sort the semitones array
        std::sort(effect.semitones.begin(), effect.semitones.begin() + effect.semitoneCount);
    }
}

void UIHandler::updateConfig() {
    // Update configuration based on UI state
    for (const auto& effect : effects) {
        if (effect.type == EffectParam::TYPE_SEMITONES) {
            // For harmonizer, convert semitones to string representation
            std::string semitonesStr = semitonesToString(effect.semitones, effect.semitoneCount);
            config.set(effect.configKey, effect.isEnabled, semitonesStr);
        } else {
            // For numeric parameters, store the raw value
            config.set(effect.configKey, effect.isEnabled, effect.currentValue);
        }
    }
}

void UIHandler::loadFromConfig() {
    for (auto& effect : effects) {
        bool isEnabled = config.contains(effect.configKey);
        effect.isEnabled = isEnabled;
        
        if (effect.type == EffectParam::TYPE_SEMITONES) {
            // For harmonizer, parse the semitones from string
            if (isEnabled) {
                std::string harmonizer = config.get<std::string>(effect.configKey, "");
                parseSemitonesString(harmonizer, effect.semitones, effect.semitoneCount);
            } else {
                effect.semitoneCount = 0;
            }
        } 
        else if (effect.type == EffectParam::TYPE_FLOAT) {
            // For float parameters
            effect.currentValue = config.get<float>(effect.configKey, 
                                                   (effect.minValue + effect.maxValue) / 2.0f);
        }
        else if (effect.type == EffectParam::TYPE_INT) {
            // For int parameters
            float value = config.get<int>(effect.configKey, 
                                        static_cast<int>((effect.minValue + effect.maxValue) / 2.0f));
            effect.currentValue = value;
        }
    }
}

std::string UIHandler::semitonesToString(const std::array<int, EffectParam::MAX_SEMITONES>& semitones, int count) {
    std::stringstream ss;
    for (int i = 0; i < count; i++) {
        ss << semitones[i];
        if (i < count - 1) {
            ss << " ";
        }
    }
    return ss.str();
}

void UIHandler::parseSemitonesString(const std::string& str, std::array<int, EffectParam::MAX_SEMITONES>& semitones, int& count) {
    std::stringstream ss(str);
    count = 0;
    int value;
    while (ss >> value && count < EffectParam::MAX_SEMITONES) {
        semitones[count++] = value;
    }
}