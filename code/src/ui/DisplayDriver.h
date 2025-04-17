#ifndef DISPLAYHANDLER_H
#define DISPLAYHANDLER_H
#include <iostream>
#include "SSD1305.h"
// Constants for piano key dimensions and positions
#define NUM_EFFECTS 3
#define KEY0_X_POSITION 62
#define WHITE_KEY_Y_POSITION 14
#define WHITE_KEY_HEIGHT 18
#define WHITE_KEY_WIDTH 6
#define BLACK_KEY_Y_POSITION 15
#define BLACK_KEY_WIDTH 4
#define BLACK_KEY_HEIGHT 8


int is_black_key(int note);
int get_white_key_x_position(int cursor);
int get_black_key_x_position(int cursor);
int get_key_x_position(int cursor);
void draw_header(void);
void draw_selection(int *semitones, int count);
void draw_cursor(int cursor, int *semitones, int count);
void update_display(int cursor, int *semitones, int count);


#endif // DISPLAYHANDLER_H
