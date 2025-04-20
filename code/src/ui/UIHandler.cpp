#include "UIHandler.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

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

bool UIHandler::init(DigitalSignalChain &dspChain) {
    // Initialize the display
    this->dspChain = &dspChain;
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
    std::cout << "Updating display..." << std::endl;
    
    // Set cursor position on piano keyboard
    display.setCursor(cursorPosition);
    
    // Set selected notes for harmonizer
    std::cout << "  Harmonizer enabled: " << (effects[0].isEnabled ? "YES" : "NO") 
             << ", semitone count: " << effects[0].semitoneCount << std::endl;
             
    if (effects[0].isEnabled && effects[0].semitoneCount > 0) {
        std::cout << "  Sending semitones to display: ";
        for (int i = 0; i < effects[0].semitoneCount; i++) {
            std::cout << effects[0].semitones[i] << " ";
        }
        std::cout << std::endl;
        
        display.setSelectedNotes(effects[0].semitones.data(), effects[0].semitoneCount);
    } else {
        std::cout << "  No semitones to display" << std::endl;
        display.setSelectedNotes(nullptr, 0);
    }
    const auto& effect = effects[currentEffectIndex];
    
    std::cout << "  Updating display with effect: " << effect.name 
             << ", value: " << effect.currentValue 
             << ", enabled: " << effect.isEnabled << std::endl;
    
    // Update the display with current effect information
    display.update(effect.name.c_str(), effect.currentValue, effect.isEnabled);
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
        // Could implement a "reset to default" feature here
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
            this->dspChain->configureEffects(config); // Apply changes to DSP chain
            
        } else {
            // For numeric parameters, store the raw value
            config.set(effect.configKey, effect.isEnabled, effect.currentValue);
        }
    }
}

void UIHandler::loadFromConfig() {
    std::cout << "=== LOADING CONFIG SETTINGS ===" << std::endl;
    
    for (auto& effect : effects) {
        // Try both approaches to get the value - directly or via string-based lookup
        bool isEnabled = false;
        
        // Check if the key exists in any form
        if (config.contains(effect.configKey)) {
            isEnabled = true;  // If contains() returns true, it's enabled by definition
            
            std::cout << "Effect: " << effect.name << " found in config, Enabled: YES" << std::endl;
            
            
            // Process value based on effect type
            if (effect.type == EffectParam::TYPE_SEMITONES) {
                std::string rawValue = config.get<std::string>(effect.configKey, "");
                std::cout << "  Raw harmonizer string: '" << rawValue << "'" << std::endl;
                
                // Parse the semitones
                parseSemitonesString(rawValue, effect.semitones, effect.semitoneCount);
                
                std::cout << "  Parsed " << effect.semitoneCount << " semitones: ";
                for (int i = 0; i < effect.semitoneCount; i++) {
                    std::cout << effect.semitones[i] << " ";
                }
                std::cout << std::endl;
            }
            else if (effect.type == EffectParam::TYPE_FLOAT || effect.type == EffectParam::TYPE_INT) {
                // For numeric values, use a consistent approach
                float defaultValue = (effect.minValue + effect.maxValue) / 2.0f;
                float value = config.get<float>(effect.configKey, defaultValue);
                
                std::cout << "  Config lookup for " << effect.name 
                          << ": key='" << effect.configKey << "'"
                          << ", returned value=" << value 
                          << ", default=" << defaultValue << std::endl;
                
                effect.currentValue = value;
            }
        } 
        else {
            std::cout << "Effect: " << effect.name << " not found in config, setting to disabled" << std::endl;
            isEnabled = false;
            
            if (effect.type == EffectParam::TYPE_SEMITONES) {
                effect.semitoneCount = 0;
                std::cout << "  Using empty semitones array" << std::endl;
            } else {
                float defaultValue = (effect.minValue + effect.maxValue) / 2.0f;
                effect.currentValue = defaultValue;
                std::cout << "  Using default value: " << defaultValue << std::endl;
            }
        }
        
        effect.isEnabled = isEnabled;
    }
    
    // After loading all effects, set the step sizes
    for (auto& effect : effects) {
        if (effect.type == EffectParam::TYPE_SEMITONES) {
            effect.stepSize = 1.0f; // Not used for harmonizer
        }
        else if (effect.type == EffectParam::TYPE_FLOAT) {
            // For Fuzz (range 1.0-6.0)
            if (effect.name == "Fuzz") {
                effect.stepSize = 0.1f; // Adjust fuzz in 0.1 increments
            }
            // For Gain (range 0.0-200.0)
            else if (effect.name == "Gain") {
                effect.stepSize = 5.0f; // Adjust gain in steps of 5
            }
            // Default fallback
            else {
                effect.stepSize = (effect.maxValue - effect.minValue) / 40.0f; // ~40 steps across range
            }
        }
        else if (effect.type == EffectParam::TYPE_INT) {
            effect.stepSize = 1.0f; // Integer parameters step by 1
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