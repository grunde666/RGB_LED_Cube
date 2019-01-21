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

 struct rgbLed
 {
     uint16_t r;
     uint16_t g;
     uint16_t b;
 };

const extern struct rgbLed ledChannel_Array[16];
extern volatile uint8_t *currentFrame;
extern volatile uint8_t *nextFrame;

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
