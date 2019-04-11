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

extern const struct hsv color_table[14];
extern struct hsv globalHSV;
extern volatile struct rgbLed *currentFrame;
extern volatile struct rgbLed *nextFrame;
extern volatile uint8_t frameReady;

uint8_t getLedColor(volatile struct rgbLed *led);
void setLedColor(volatile struct rgbLed *led, const struct hsv *newHSV);

uint8_t fadeColorCube(uint8_t replay);
uint8_t rainfall(uint8_t replay);
uint8_t fillCubeDiagonal(uint8_t replay);
uint8_t randomLedColorCube(uint8_t replay);
uint8_t activateRandomLED(uint8_t replay);
uint8_t fillCube_randomly(uint8_t replay);
uint8_t clearCube_randomly(uint8_t replay);
uint8_t dropLedTopDown(uint8_t replay);
//void explosion(uint8_t replay);
//void rotateLayer(uint8_t replay);
//void cubeFraming(uint8_t replay);
uint8_t floatingXLayer(uint8_t replay);
uint8_t floatingYLayer(uint8_t replay);
uint8_t floatingZLayer(uint8_t replay);
uint8_t everyLED(void);

//void blinkingCube(uint8_t replays);
#endif // ANIMATIONS_H_INCLUDED
