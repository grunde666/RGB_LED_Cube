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

enum color_type
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

 struct rgbLed
 {
     uint16_t r;
     uint16_t g;
     uint16_t b;
 };

extern const struct rgbLed color[8];
extern const uint16_t pwmtable_12[128];
volatile struct rgbLed *currentFrame;
volatile struct rgbLed *nextFrame;

inline void setRGBLed(volatile struct rgbLed *led, uint8_t colorValue, uint8_t dimmingLevel)
{
    led->r = color[colorValue].r * pwmtable_12[dimmingLevel];
    led->g = color[colorValue].g * pwmtable_12[dimmingLevel];
    led->b = color[colorValue].b * pwmtable_12[dimmingLevel];
}

uint8_t getRGBLedColor(volatile struct rgbLed *led);
void blinkingCube(uint8_t replays);
void rainfall(uint16_t frameCt);
void fillCubeDiagonal(uint8_t replays);
void rain(uint8_t replay);
void activateRandomLED(uint8_t replay);
void fillCube_randomly(uint8_t replay);
void clearCube_randomly(uint8_t replay);
void dropLedTopDown(uint8_t replay);
void explosion(uint8_t replay);
void rotateLayer(uint8_t replay);
void cubeFraming(uint8_t replay);
void floatingXLayer(uint8_t replay);
void floatingYLayer(uint8_t replay);
void floatingZLayer(uint8_t replay);
void randomFill(uint16_t frameCt);
void everyLED(void);

//void blinkingCube(uint8_t replays);
#endif // ANIMATIONS_H_INCLUDED
