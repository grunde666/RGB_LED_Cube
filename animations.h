#ifndef ANIMATIONS_H_INCLUDED
#define ANIMATIONS_H_INCLUDED

#include <avr/io.h>

#define X_LAYER 0
#define Y_LAYER 1
#define Z_LAYER 2

#define BOTTOM  0
#define TOP     3

#define LEFT    0
#define RIGHT   3

#define BACK    0
#define FRONT   3

enum rgb_color_type
{
    RGB_COLOR_BLACK,
    RGB_COLOR_BLUE,
    RGB_COLOR_GREEN,
    RGB_COLOR_CYAN,
    RGB_COLOR_RED,
    RGB_COLOR_MAGENTA,
    RGB_COLOR_YELLOW,
    RGB_COLOR_WHITE
};

enum hsv_color_type
{
    HSV_COLOR_RED,
    HSV_COLOR_ORANGE,
    HSV_COLOR_YELLOW,
    HSV_COLOR_VIOLET,
    HSV_COLOR_BLACK,
    HSV_COLOR_BLUE,
    HSV_COLOR_BROWN,
    HSV_COLOR_WHITE,
    HSV_COLOR_GREEN,
    HSV_COLOR_CYAN,
    HSV_COLOR_MAGENTA,
    HSV_COLOR_BLUEGREEN,
    HSV_COLOR_GREENBLUE,
    HSV_COLOR_GREENYELLOW,
    HSV_COLOR_BLUERED
};

 struct rgbLed
 {
     uint16_t r;
     uint16_t g;
     uint16_t b;
 };

 struct hsv
 {
    uint8_t h;
    uint8_t s;
    uint8_t v;
 };

extern const struct hsv color_table[15];
volatile struct rgbLed *currentFrame;
volatile struct rgbLed *nextFrame;

uint8_t getLedColor(volatile struct rgbLed *led);
void setLedColor(volatile struct rgbLed *led, const struct hsv *newHSV);

void fadeColorCube(uint8_t replays);
void rainfall(uint8_t replays);
void fillCubeDiagonal(uint8_t replays);
void activateRandomLED(uint8_t replay);
void fillCube_randomly(uint8_t replay);
void clearCube_randomly(uint8_t replay);
void dropLedTopDown(uint8_t replay);
//void explosion(uint8_t replay);
//void rotateLayer(uint8_t replay);
//void cubeFraming(uint8_t replay);
void floatingXLayer(uint8_t replay);
void floatingYLayer(uint8_t replay);
void floatingZLayer(uint8_t replay);
void everyLED(void);

//void blinkingCube(uint8_t replays);
#endif // ANIMATIONS_H_INCLUDED
