#include "DisplayDriver.h"
#include "DEV_Config.h"
// Look-up arrays for white and black key semitones
static const int white_keys[21] = {-17, -15, -13, -12, -10, -8, -7, -5, -3, -1, 0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17};
static const int black_keys[14] = {-16, -14, -11, -9, -6, -4, -2, 1, 3, 6, 8, 10, 13, 15};
// Pixel location references for drawing over keys
static const int black_key_positions[14] = {5, 12, 23, 30, 41, 47, 54, 65, 72, 83, 89, 96, 107, 114};

//Variables for effects - will need refactored later
const char* effect_names[NUM_EFFECTS] = {"FX:Fuzz", "FX:Pitch", "FX:+ Octave"};
bool effect_states[NUM_EFFECTS] = {false, false, false}; // All effects start off
int current_effect = 0; // Start with the first effect
int semitones[4] = {0,3,7, -12}; 
int count = 4;


// Check if a note is a black key. Return 1 if true, 0 if false.
int is_black_key(int note) {
    for (int i = 0; i < 14; i++) {
        if (black_keys[i] == note) {
            return 1; 
        }
    }
    return 0; 
}

// Get white key x position based on cursor value
int get_white_key_x_position(int cursor) {
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
int get_black_key_x_position(int cursor) {
    for (int i = 0; i < 14; i++) {
        if (black_keys[i] == cursor) {
            return black_key_positions[i];
        }
    }
    // If not found, return default
    return KEY0_X_POSITION;
}

// Return the upper left x position of any key based on cursor value
int get_key_x_position(int cursor) {
    if (is_black_key(cursor)) {
        return get_black_key_x_position(cursor);
    } else {
        return get_white_key_x_position(cursor);
    }
}

void draw_header(){
    if (effect_states[current_effect]) {
        // Draw a filled white rectangle as background
        for (int i = 2; i < 62; i++) {
            for (int j = 2; j < 2 + 8; j++) {
                SSD1305_pixel(i, j, 1);
            }
        }
        // Then draw black text on white background
        SSD1305_string_4x7(3, 3, effect_names[current_effect], 0);
    } else {
        // Normal rendering for ON state
        SSD1305_string_4x7(3, 3, effect_names[current_effect], 1);
    }
    SSD1305_string_4x7(67, 3, "Shift: + 12", 1);
}

void draw_selection(int *semitones, int count){
    // Draw the selection rectangle for each semitone
    for (int i = 0; i < count; i++) {
        int x_pos = get_key_x_position(semitones[i]);
        if (is_black_key(semitones[i])) {
            int y_pos = BLACK_KEY_Y_POSITION;
            for (int j = x_pos + 1; j < x_pos + BLACK_KEY_WIDTH - 1; j++) {
                for (int k = y_pos + 1; k < y_pos + BLACK_KEY_HEIGHT - 1; k++) {
                    SSD1305_pixel(j, k, 1); // Set to white
                }
            }
        }
        else{
            // Draw a filled rectangle for the selection
            int y_pos = WHITE_KEY_Y_POSITION;
            // Draw a filled rectangle for the selection
            for (int j = x_pos + 1; j < x_pos + WHITE_KEY_WIDTH - 2; j++) {
                for (int k = y_pos + 10; k < y_pos + WHITE_KEY_HEIGHT - 2; k++) {
                    SSD1305_pixel(j, k, 0); // Set to white
                }
            }
        }
    }
}

void draw_cursor(int cursor, int *semitones, int count) {
    // Check if cursor is on a black key
    int is_black = is_black_key(cursor);

    
    // Get position to draw cursor
    char cursor_x = get_key_x_position(cursor);
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

    if(is_black) {
        for (int k = 0; k < count; k++) { 
            if (semitones[k] == cursor) {
                for (int i = cursor_x; i < cursor_x + cursor_width; i++) {
                    for (int j = cursor_y -1; j < cursor_y + 1; j++) {    
                        if ((i + j) % 2 == 0) {
                            SSD1305_pixel(i, j, 1);  // Set to white on black keys
                        } else {
                            SSD1305_pixel(i, j, 0);  // Set to black on black keys
                        }
                        }
                    }
                return;  // Exit function - don't draw cursor on selected keys
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

void update_display(int cursor, int *semitones, int count) {

    SSD1305_clear();
    SSD1305_bitmap(0, 0, piano_screen, 128, 32, 1);
    draw_cursor(cursor, semitones, count);
    draw_selection(semitones, count);
    draw_header();
    SSD1305_display();
}

/* int main(int argc, char *argv[])
{

    if(DEV_ModuleInit() != 0) {
        return -1;
    }
    SSD1305_begin();
    SSD1305_clear();

    int cursor = -17;
    int counter = 0;
    int effect_toggle_counter = 0;
    
    while (1)
    {
        // Increment counters each time we sleep
        counter++;
        effect_toggle_counter++;
        // Check if it's time to change cursor position
        if (counter >= 5) // ~1 second with 100ms sleeps
        {
            // Reset counter
            counter = 0;
            // Move to next semitone
            cursor++;
            // Wrap around if we reach the upper limit
            if (cursor > 17)
            {
                cursor = -17;
            }
            // Draw cursor at new position
            update_display(cursor, semitones, count);

        }
        // Check if it's time to toggle effect state or cycle to next effect
        if (effect_toggle_counter >= 20) // ~2 seconds with 100ms sleeps
        {
            effect_toggle_counter = 0;
            // Toggle current effect state
            effect_states[current_effect] = !effect_states[current_effect];
            // If turning off, move to next effect
            if (!effect_states[current_effect]) {
                current_effect = (current_effect + 1) % NUM_EFFECTS;
            }
            // Redraw with updated effect state
            update_display(cursor, semitones, count);
        }
        usleep(100000); // 100ms sleep
    }
    return 0;
}
 */