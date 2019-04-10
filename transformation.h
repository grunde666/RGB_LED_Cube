#ifndef TRANSFORMATION_H_INCLUDED
#define TRANSFORMATION_H_INCLUDED

#include "animations.h"
#include <avr/io.h>
void moveLayerDown(uint8_t clearLEDs);
void moveLayerUp(uint8_t clearLEDs);
void moveLayerLeft(uint8_t clearLEDs);
void moveLayerRight(uint8_t clearLEDs);
void moveLayerForward(uint8_t clearLEDs);
void moveLayerBackward(uint8_t clearLEDs);
void shiftDownward(void);
void shiftUpward(void);
void shiftForward(void);
void shiftBackward(void);
void shiftLeft(void);
void shiftRight(void);
void clearLEDCube(void);
void fillLEDCube(struct hsv *newHSV);
void copyFrame(void);
void copyLayer(uint8_t layerType, uint8_t originLayer, uint8_t destinationLayer);
void fillLayer(uint8_t layerType, uint8_t layerNumber, struct hsv *newHSV);
void clearLayer(uint8_t layerType, uint8_t layerNumber);

#endif // TRANSFORMATION_H_INCLUDED
