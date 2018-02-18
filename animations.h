#ifndef ANIMATIONS_H_INCLUDED
#define ANIMATIONS_H_INCLUDED

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define DEBUG 0

#if DEBUG
#define SAEULE1 PB3
#define SAEULE2 PC3
#define SAEULE3 PC1
#define SAEULE4 PB5
#define SAEULE5 PC0
#define SAEULE6 PC5
#define SAEULE7 PC4
#define SAEULE8 PB4
#define SAEULE9 PC2

#define EBENE_UNTEN PD5
#define EBENE_MITTE PD7
#define EBENE_OBEN  PD6

#else
#define SAEULE1 PB5
#define SAEULE2 PB3
#define SAEULE3 PB4
#define SAEULE4 PC2
#define SAEULE5 PC1
#define SAEULE6 PC0
#define SAEULE7 PC5
#define SAEULE8 PC4
#define SAEULE9 PC3

#define EBENE_UNTEN PD5
#define EBENE_MITTE PD6
#define EBENE_OBEN  PD7

#endif

#define OBEN 2
#define MITTE 1
#define UNTEN 0

#define LINKS 2
#define RECHTS 0

#define HINTEN 2
#define VORNE 0

#define X_LAYER 0
#define Y_LAYER 1
#define Z_LAYER 2

#define WARTEZEIT 5

extern uint8_t cube_array[3][3];

void blinkingCube(uint8_t replays);
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

void set_pixelStatus(uint8_t x, uint8_t y, uint8_t z, uint8_t status);
uint8_t get_pixelStatus(uint8_t x, uint8_t y, uint8_t z);
void moveLayerDown(uint8_t clearLEDs);
void moveLayerUp(uint8_t clearLEDs);
void moveLayerLeft(uint8_t clearLEDs);
void moveLayerRight(uint8_t clearLEDs);
void moveLayerForward(uint8_t clearLEDs);
void moveLayerBackward(uint8_t clearLEDs);
void waitForFrame(uint16_t maxNumber);
void clearLEDCube(void);
void fillLEDCube(void);
void copyLayer(uint8_t layerType, uint8_t originLayer, uint8_t destinationLayer);
void fillLayer(uint8_t layerType, uint8_t layerNumber);
void clearLayer(uint8_t layerType, uint8_t layerNumber);

#endif // ANIMATIONS_H_INCLUDED
