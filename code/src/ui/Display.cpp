#include "Display.h"

Display::Display() : cursor(0), semitones(nullptr), semitone_count(0) {
    // Initialize with default values
}

// Destructor
Display::~Display() {
    // Clean up resources if needed
}

// Initialize display hardware
bool Display::init() {
    if(DEV_ModuleInit() != 0) {
        return false;
    }
    SSD1305_begin();
    SSD1305_clear();
    return true;
}

// Check if a note is a black key
bool Display::isBlackKey(int note) {
    for (int i = 0; i < 14; i++) {
        if (black_keys[i] == note) {
            return true;
        }
    }
    return false;
}

// Get white key x position based on cursor value
int Display::getWhiteKeyXPosition(int cursor) {
    int cursor_index = -1;
    int zero_index = -1;

    // Find the indices of cursor and 0 in white_keys array
    for (int i = 0; i < 21; i++) {
        if (white_keys[i] == cursor) {
            cursor_index = i;
        }
        if (white_keys[i] == 0) {
            zero_index = i;
        }
    }

    if (cursor_index == -1) {
        // Cursor value not found in array, return default position
        return KEY0_X_POSITION;
    }

    // Calculate offset based on position difference in white_keys
    int key_offset = cursor_index - zero_index;

    // Each white key is 6 pixels wide
    return KEY0_X_POSITION + (key_offset * 6);
}

// Get black key x position based on cursor value
int Display::getBlackKeyXPosition(int cursor) {
    for (int i = 0; i < 14; i++) {
        if (black_keys[i] == cursor) {
            return black_key_positions[i];
        }
    }
    // If not found, return default
    return KEY0_X_POSITION;
}

// Return the upper left x position of any key based on cursor value
int Display::getKeyXPosition(int cursor) {
    if (isBlackKey(cursor)) {
        return getBlackKeyXPosition(cursor);
    } else {
        return getWhiteKeyXPosition(cursor);
    }
}

void Display::drawHeader(const char* effectName, float effectValue, bool isEnabled) {
    if (isEnabled) {
        // Draw a filled white rectangle as background
        for (int i = 2; i < 62; i++) {
            for (int j = 2; j < 2 + 8; j++) {
                SSD1305_pixel(i, j, 1);
            }
        }
        // Then draw black text on white background
        SSD1305_string_4x7(3, 3, effectName, 0);
    } else {
        // Normal rendering for OFF state
        SSD1305_string_4x7(3, 3, effectName, 1);
    }
    char effectValueStr[10];
    snprintf(effectValueStr, sizeof(effectValueStr), "%.2f", effectValue);
    SSD1305_string_4x7(67, 3, effectValueStr, 1);
}

// Draw selected notes
void Display::drawSelection() {
    if (!semitones || semitone_count <= 0) {
        return;
    }
    
    // Draw the selection rectangle for each semitone
    for (int i = 0; i < semitone_count; i++) {
        int x_pos = getKeyXPosition(semitones[i]);
        if (isBlackKey(semitones[i])) {
            int y_pos = BLACK_KEY_Y_POSITION;
            for (int j = x_pos + 1; j < x_pos + BLACK_KEY_WIDTH - 1; j++) {
                for (int k = y_pos + 1; k < y_pos + BLACK_KEY_HEIGHT - 1; k++) {
                    SSD1305_pixel(j, k, 1); // Set to white
                }
            }
        } else {
            // Draw a filled rectangle for the selection
            int y_pos = WHITE_KEY_Y_POSITION;
            for (int j = x_pos + 1; j < x_pos + WHITE_KEY_WIDTH - 2; j++) {
                for (int k = y_pos + 10; k < y_pos + WHITE_KEY_HEIGHT - 2; k++) {
                    SSD1305_pixel(j, k, 0); // Set to black
                }
            }
        }
    }
}

// Draw cursor at current position
void Display::drawCursor() {
    // Check if cursor is on a black key
    bool is_black = isBlackKey(cursor);
    
    // Get position to draw cursor
    char cursor_x = getKeyXPosition(cursor);
    
    // Adjust settings based on key type
    char cursor_y, cursor_width, cursor_height;
    if (is_black) {
        // Black key settings
        cursor_y = BLACK_KEY_Y_POSITION;
        cursor_width = BLACK_KEY_WIDTH;
        cursor_height = BLACK_KEY_HEIGHT;
    } else {
        // White key settings
        cursor_y = WHITE_KEY_Y_POSITION;
        cursor_width = WHITE_KEY_WIDTH;
        cursor_height = WHITE_KEY_HEIGHT;
    }

    // Check if cursor is on a selected note (for black keys)
    if (is_black && semitones) {
        for (int k = 0; k < semitone_count; k++) { 
            if (semitones[k] == cursor) {
                for (int i = cursor_x; i < cursor_x + cursor_width; i++) {
                    for (int j = cursor_y - 1; j < cursor_y + 1; j++) {    
                        if ((i + j) % 2 == 0) {
                            SSD1305_pixel(i, j, 1);  // Set to white on black keys
                        } else {
                            SSD1305_pixel(i, j, 0);  // Set to black on black keys
                        }
                    }
                }
                return;  // Exit function - don't draw standard cursor on selected keys
            }
        }
    } 

    // Draw checkered pattern
    for (int i = cursor_x; i < cursor_x + cursor_width; i++) {
        for (int j = cursor_y; j < cursor_y + cursor_height; j++) {
            // For black keys, we need special handling to maintain borders
            if (is_black) {
                // Skip the border pixels
                if (i == cursor_x || i == cursor_x + cursor_width - 1 || j == cursor_y + cursor_height - 1) {
                    // Don't modify border pixels
                    continue;
                }
                // Draw alternating white and black pixels for the interior
                if ((i + j) % 2 == 0) {
                    SSD1305_pixel(i, j, 1);  // Set to white on black keys
                } else {
                    SSD1305_pixel(i, j, 0);  // Set to black on black keys
                }
            } else {
                // For white keys, just set black pixels for checkerboard
                if ((i + j) % 2 == 0) {
                    SSD1305_pixel(i, j, 0);  // Set to black
                }
            }
        }
    }
}

void Display::update(const char* effectName, float effectValue, bool isEnabled) {
    SSD1305_clear();
    SSD1305_bitmap(0, 0, piano_screen, 128, 32, 1);
    drawCursor();
    drawSelection();
    drawHeader(effectName, effectValue, isEnabled);
    SSD1305_display();
}

// Set cursor position
void Display::setCursor(int note) {
    cursor = note;
}

// Set selected notes
void Display::setSelectedNotes(int* notes, int count) {
    semitones = notes;
    semitone_count = count;
}
