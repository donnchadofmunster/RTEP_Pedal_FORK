#ifndef DISPLAY_H
#define DISPLAY_H

#include "SSD1305.h"
#include "DEV_Config.h"

// Keyboard position constants
#define KEY0_X_POSITION 59  // Position of the central C key
#define WHITE_KEY_Y_POSITION 12
#define WHITE_KEY_WIDTH 6
#define WHITE_KEY_HEIGHT 19
#define BLACK_KEY_Y_POSITION 12
#define BLACK_KEY_WIDTH 4
#define BLACK_KEY_HEIGHT 12

// Forward declare external bitmap
extern const unsigned char piano_screen[];

class Display {
private:
    // Static keyboard layout data
    static constexpr int white_keys[21] = {-17, -15, -13, -12, -10, -8, -7, -5, -3, -1, 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17};
    static constexpr int black_keys[14] = {-16, -14, -11, -9, -6, -4, -2, 1, 3, 6, 8, 10, 13, 15};
    static constexpr int black_key_positions[14] = {5, 12, 23, 30, 41, 47, 54, 65, 72, 83, 89, 96, 107, 114};
    
    // Note tracking
    int cursor;
    int* semitones;
    int semitone_count;

    // Helper methods for drawing
    bool isBlackKey(int note);
    int getWhiteKeyXPosition(int cursor);
    int getBlackKeyXPosition(int cursor);
    int getKeyXPosition(int cursor);
    void drawHeader(const char* effectName, float effectValue, bool isEnabled);
    void drawSelection();
    void drawCursor();

public:
    // Constructor & destructor
    Display();
    ~Display();
    
    // Initialization
    bool init();
    
    // State setting methods
    void setCursor(int note);
    void setSelectedNotes(int* notes, int count);
    
    // Display update method with effect information provided by UIHandler
    void update(const char* effectName, float effectValue, bool isEnabled);
};

#endif // DISPLAY_H